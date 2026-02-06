# Safe Proxy Site

A full-stack Node.js + Express proxy app with:

- Domain allowlist from environment variables (prevents open proxy abuse)
- Proxy support for HTTP, HTTPS, WS, and WSS targets
- Rate limiting on `/proxy`
- CORS support
- Security headers via Helmet
- Front-end UI at `/` for proxying a target URL
- Deployment support for Replit/Railway (via `.replit` + Dockerfile)

## Quick start

```bash
cp .env.example .env
npm install
npm start
```

Then open `http://localhost:3000`.

## Environment variables

- `PROXY_ALLOWLIST` (required): comma-separated domains, e.g. `example.com,api.github.com`
- `CORS_ORIGINS` (optional): comma-separated allowed origins
- `RATE_WINDOW_MS` (optional): rate-limit window in ms (default `60000`)
- `RATE_MAX` (optional): requests per window per IP (default `60`)
- `PORT` (optional): listen port

## Proxy endpoint

`GET /proxy?url=<encoded_target_url>`

The target URL is validated against `PROXY_ALLOWLIST`, must use one of `http`, `https`, `ws`, or `wss`, and cannot be localhost/private/IP-literal destinations.

## Deploy

### Replit
- `.replit` and `replit.nix` are included.
- Add environment variables in Replit Secrets.

### Railway (or other container hosts)
- Deploy with the provided `Dockerfile`.
- Set environment variables in project settings.
