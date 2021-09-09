# Producer-Subscriber

This repo contains an implementation of the producer-subscriber architecture for distributed apps. The server (broker) is implemented in C, and the client is implemented in Java.

The server uses some additional services, such as as storage server accessed using RPCs, and a web service for formatting dates, accessed using Java Web Services. The RPC storage server code, and the formatter web service is also here.

## Compiation

All the compilation is handled by the Makefile. To compile everything, just run `make` in the root directory of the project

Running `make clean` removes all the generated files, returning the repo to its initial state

## Deployment

The server processes can be run in background, however, some of them print some traces in the console, so running them on different terminals or redirecting the output to /dev/null is recommented.

The `launchAll` script launches all the server processes, putting some of them in background. To stop them later, use the `stopAll` script.

This are the commands used to run all the components, in the order in which they have to be run:

RPC storage server:
```
./servidorAlmacenamiento
```

Broker:
```
./broker -p <listenPort> -r <rpcServerIP>
```

"Formatter" Web service:
```
java -cp . formatter.formatterPublisher <port>
```

Subscriptor:
```
java -cp . suscriptor.suscriptor -s <brokerIP> -p <brokerPort> -w <formatterIP> -q <formatterPort>
```

Editor:
```
./editor -h <brokerIP> -p <brokerPort> -t "topic" -m "message"
```

The topic and the message can contain spaces. If any of this arguments doesn't contain spaces, they can be written without the quotes
