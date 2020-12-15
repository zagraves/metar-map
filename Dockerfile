FROM node:14.15.1 AS build-env

ENV WORKDIR /app
COPY . $WORKDIR
WORKDIR $WORKDIR

FROM gcr.io/distroless/nodejs:14
COPY --from=build-env $WORKDIR $WORKDIR
WORKDIR $WORKDIR

ENV NODE_NO_WARNINGS 1

ENTRYPOINT ["node", "--experimental-modules", "--es-module-specifier-resolution=node", "src/index.js"]
