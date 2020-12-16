FROM node:14.15.1 AS build-env

ENV WORKDIR /app
COPY . $WORKDIR
WORKDIR $WORKDIR

FROM arm64v8/node:14.15-slim
COPY --from=build-env $WORKDIR $WORKDIR
WORKDIR /app

ENV NODE_NO_WARNINGS 1

ENTRYPOINT ["node", "--experimental-modules", "--es-module-specifier-resolution=node", "src/index.js"]
