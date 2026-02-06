import express from "express";
import { createProxyMiddleware } from "http-proxy-middleware";
import dotenv from "dotenv";
import helmet from "helmet";
import morgan from "morgan";
import compression from "compression";
import cors from "cors";
import rateLimit from "express-rate-limit";

dotenv.config();

const app = express();
const PORT = Number(process.env.PORT || 3000);

const ALLOWLIST = (process.env.PROXY_ALLOWLIST || "")
  .split(",")
  .map((entry) => entry.trim().toLowerCase())
  .filter(Boolean);

if (ALLOWLIST.length === 0) {
  console.error("[startup-error] PROXY_ALLOWLIST is empty. Configure allowed domains in your environment.");
  process.exit(1);
}

const CORS_ORIGINS = (process.env.CORS_ORIGINS || "")
  .split(",")
  .map((entry) => entry.trim())
  .filter(Boolean);

app.set("trust proxy", process.env.TRUST_PROXY === "true" ? 1 : false);
app.use(helmet());
app.use(compression());
app.use(express.json({ limit: "1mb" }));
app.use(express.urlencoded({ extended: true }));
app.use(morgan("combined"));
app.use(
  cors(
    CORS_ORIGINS.length > 0
      ? {
          origin(origin, callback) {
            if (!origin || CORS_ORIGINS.includes(origin)) {
              return callback(null, true);
            }

            return callback(new Error("Origin not allowed by CORS"));
          },
        }
      : undefined,
  ),
);

app.use(
  "/proxy",
  rateLimit({
    windowMs: Number(process.env.RATE_WINDOW_MS || 60_000),
    max: Number(process.env.RATE_MAX || 60),
    standardHeaders: true,
    legacyHeaders: false,
  }),
);

app.use(express.static("public"));

function isPrivateOrIpLiteral(hostname) {
  if (!hostname) return true;

  const ipv4 = /^(\d{1,3}\.){3}\d{1,3}$/;
  if (ipv4.test(hostname)) return true;

  if (hostname.includes(":")) return true;

  const lower = hostname.toLowerCase();
  if (lower === "localhost" || lower.endsWith(".localhost")) return true;

  return false;
}

function parseAndValidateTarget(target) {
  if (!target) {
    return { ok: false, reason: "Missing 'url' query parameter" };
  }

  let parsed;
  try {
    parsed = new URL(target);
  } catch {
    return { ok: false, reason: "Invalid URL" };
  }

  const protocol = parsed.protocol.toLowerCase();
  if (!["http:", "https:", "ws:", "wss:"].includes(protocol)) {
    return { ok: false, reason: "Only HTTP, HTTPS, WS, and WSS are allowed" };
  }

  const hostname = parsed.hostname.toLowerCase();
  if (isPrivateOrIpLiteral(hostname)) {
    return { ok: false, reason: "Private hosts and IP literals are blocked" };
  }

  const allowlisted = ALLOWLIST.some(
    (domain) => hostname === domain || hostname.endsWith(`.${domain}`),
  );

  if (!allowlisted) {
    return { ok: false, reason: "Domain is not in PROXY_ALLOWLIST" };
  }

  return { ok: true, target: parsed };
}

app.get("/healthz", (_req, res) => {
  res.json({ ok: true, uptime: process.uptime() });
});

const proxy = createProxyMiddleware({
  changeOrigin: true,
  ws: true,
  secure: true,
  followRedirects: true,
  target: "https://example.com",
  router(req) {
    const validation = parseAndValidateTarget(String(req.query.url || ""));
    if (!validation.ok) {
      throw new Error(`REJECTED_TARGET:${validation.reason}`);
    }

    return validation.target.origin;
  },
  pathRewrite(_path, req) {
    const validation = parseAndValidateTarget(String(req.query.url || ""));
    if (!validation.ok) {
      return "/";
    }

    return `${validation.target.pathname}${validation.target.search}`;
  },
  onProxyReq(proxyReq) {
    proxyReq.setHeader("X-Forwarded-By", "safe-proxy-site");
    [
      "connection",
      "keep-alive",
      "proxy-authenticate",
      "proxy-authorization",
      "te",
      "trailers",
      "transfer-encoding",
      "upgrade",
    ].forEach((header) => proxyReq.removeHeader(header));
  },
  onError(error, _req, res) {
    if (error.message.startsWith("REJECTED_TARGET:")) {
      res.status(400).json({ error: error.message.replace("REJECTED_TARGET:", "") });
      return;
    }

    if (!res.headersSent) {
      res.status(502).json({ error: "Proxy request failed", detail: error.message });
    }
  },
});

app.use("/proxy", (req, res, next) => {
  const validation = parseAndValidateTarget(String(req.query.url || ""));
  if (!validation.ok) {
    res.status(400).json({ error: validation.reason });
    return;
  }

  proxy(req, res, next);
});

app.use((_req, res) => {
  res.status(404).json({ error: "Not Found" });
});

app.listen(PORT, () => {
  console.log(`safe-proxy-site running on http://0.0.0.0:${PORT}`);
  console.log(`Allowlist domains: ${ALLOWLIST.join(", ")}`);
});
