# Safe Proxy Site (Allowlisted)

Educational reverse proxy with:
- HTTP(S) + WebSocket support
- Allowlisted upstreams (no open proxy)
- Rate limiting, gzip, CORS, security headers
- Tiny test UI at `/`

> **Use responsibly.** This project is for learning and legitimate development. Do not use it to bypass school/work restrictions or access content you’re not allowed to.

## Run locally
```bash
npm i
cp .env.example .env   # edit PROXY_ALLOWLIST
npm run dev
# http://localhost:3000
