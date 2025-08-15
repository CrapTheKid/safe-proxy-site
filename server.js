import express from "express";
import { createProxyMiddleware } from "http-proxy-middleware";
import dotenv from "dotenv";
import helmet from "helmet";
import morgan from "morgan";
import compression from "compression";
import cors from "cors";
import rateLimit from "express-rate-limit";
import URL from "url-parse";

dotenv.config();

const app = express();

// --- Config ---
const PORT = process.env.PORT || 3000; // Replit/GCP/Fly set PORT for you
const DEFAULT_TARGET = process.env.DEFAULT_TARGET || "https://example.com";

const ALLOWLIST = (process.env.PROXY_ALLOWLIST || "")
  .split(",")
  .map(s => s.trim().toLowerCase())
  .filter(Boolean);

if (ALLOWLIST.length === 0) {
  console.error("[!] PROXY_ALLOWLIST is empty. Add at least one host in .env or Replit Secrets.");
  process.exit(1);
}

const CORS_ORIGINS = (process.env.CORS_ORIGINS || "")
  .split(",")
  .map(s => s.trim())
  .filter(Boolean);

// --- Middlewares ---
app.set("trust proxy", true);
app.use(helmet({ contentSecurityPolicy: false }));
app.use(compression());
app.use(express.json({ limit: "200kb" }));
app.use(express.urlencoded({ extended: true, limit: "200kb" }));
app.use(morgan("combined"));

if (CORS_ORIGINS.length) {
  app.use(cors({ origin: CORS_ORIGINS, credentials: false }));
}

// Rate limit the proxy path
const limiter = rateLimit({
  windowMs: Number(process.env.RATE_WINDOW_MS || 60_000),
  max: Number(process.env.RATE_MAX || 120),
  standardHeaders: true,
  legacyHeaders: false
});
app.use("/proxy", limiter);

// --- Helpers ---
const PRIVATE_NET_RANGES = [
  /^10\./, /^127\./, /^169\.254\./,
  /^172\.(1[6-9]|2[0-9]|3[0-1])\./,
  /^192\.168\./, /^::1$/, /^fc00:/, /^fe80:/
];

function isPrivateHost(host) {
  if (!host) return true;
  const h = host.toLowerCase();
  if (/^\d{1,3}(\.\d{1,3}){3}$/.test(h)) {
    return PRIVATE_NET_RANGES.some(rx => rx.test(h));
  }
  if (/^[a-f0-9:]+$/i.test(h)) return true; // raw IPv6 literal blocked
  return false;
}

function allowedTarget(urlStr) {
  try {
    const parsed = new URL(urlStr);
    const host = (parsed.host || "").toLowerCase();
    if (!host) return { ok: false, reason: "No host" };
    if (isPrivateHost(host)) return { ok: false, reason: "Private/invalid host" };
    const hostname = host.split(":")[0];
    const ok = ALLOWLIST.some(allowed => hostname === allowed || hostname.endsWith(`.${allowed}`));
    return ok ? { ok: true, url: parsed.toString() } : { ok: false, reason: "Host not in allowlist" };
  } catch {
    return { ok: false, reason: "Bad URL" };
  }
}

// --- Static UI ---
app.use(express.static("public"));

// Health
app.get("/healthz", (_, res) => res.json({ ok: true, time: new Date().toISOString() }));

// --- Proxy route ---
const proxyMiddleware = createProxyMiddleware({
  target: DEFAULT_TARGET,
  changeOrigin: true,
  ws: true,
  followRedirects: true,
  secure: true,
  selfHandleResponse: false,
  onProxyReq(proxyReq) {
    [
      "connection","keep-alive","proxy-authenticate","proxy-authorization",
      "te","trailers","transfer-encoding","upgrade"
    ].forEach(h => proxyReq.removeHeader(h));
    proxyReq.setHeader("X-Forwarded-By", "safe-proxy-site");
  },
  onProxyRes(proxyRes) {
    ["server", "x-powered-by"].forEach(h => proxyRes.headers && delete proxyRes.headers[h]);
  },
  router: (req) => {
    const urlParam = req.query.url;
    if (!urlParam) return DEFAULT_TARGET;
    const check = allowedTarget(String(urlParam));
    if (!check.ok) throw new Error(`REJECT:${check.reason}`);
    return String(urlParam);
  },
  pathRewrite: (path, req) => {
    const urlParam = req.query.url;
    if (!urlParam) return path;
    const u = new URL(String(urlParam));
    return u.pathname + (u.query || "");
  },
  onError(err, req, res) {
    if (String(err.message).startsWith("REJECT:")) {
      res.status(400).json({ error: "Target rejected", reason: err.message.replace("REJECT:", "") });
    } else {
      if (!res.headersSent) res.status(502).json({ error: "Bad gateway", detail: err.message });
    }
  }
});

app.use("/proxy", (req, res, next) => {
  const allowedMethods = ["GET","POST","PUT","PATCH","DELETE","HEAD","OPTIONS"];
  if (!allowedMethods.includes(req.method)) return res.status(405).send("Method Not Allowed");
  if (req.query.url) {
    const check = allowedTarget(String(req.query.url));
    if (!check.ok) return res.status(400).json({ error: "Target rejected", reason: check.reason });
  }
  return proxyMiddleware(req, res, next);
});

app.use((req, res) => res.status(404).json({ error: "Not found" }));

app.listen(PORT, () => {
  console.log(`Proxy site on http://localhost:${PORT}`);
  console.log(`Allowlist: ${ALLOWLIST.join(", ")}`);
});
