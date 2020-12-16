FROM node:14.15.1 AS build-env

ENV WORKDIR /app

COPY ./node_modules $WORKDIR/node_modules
COPY ./package.json $WORKDIR/package.json
COPY ./package-lock.json $WORKDIR/package-lock.json
COPY ./data $WORKDIR/data
COPY ./src $WORKDIR/src
COPY ./config $WORKDIR/config

WORKDIR $WORKDIR

# FROM arm64v8/node:14.15-slim
# FROM node:14.15-alpine
FROM gcr.io/distroless/nodejs:14
COPY --from=build-env $WORKDIR $WORKDIR
WORKDIR /app

ENV NODE_NO_WARNINGS 1

ENTRYPOINT ["node", "--experimental-modules", "--es-module-specifier-resolution=node", "src/index.js"]
