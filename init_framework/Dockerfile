FROM gcc:11

WORKDIR /app

COPY include/ /app/include/
COPY src/ /app/src/
COPY test/ /app/test/
COPY Makefile /app/

RUN apt-get update && apt-get install -y make

RUN make clean && make all

CMD ["./bin/game_server"]
