rm ./font-maker
make
./font-maker 

# docker run --rm -it -v "$(pwd)":/root/ font-maker-image /bin/bash ./run.sh