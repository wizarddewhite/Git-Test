FROM l.gcr.io/google/bazel:latest

WORKDIR /build
COPY . /build

RUN bazel run //api:api-container
