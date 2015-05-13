SDCC_OPTS="--int-long-reent --float-reent --no-xinit-opt --peep-return --peep-asm --opt-code-speed $*"
IRAM_SIZE=256
XRAM_SIZE=1024
CODE_SIZE=60000
export SDCC_INCLUDE="../include"
export SDCC_LIB="./"

rm -rf build bin
mkdir build bin

cd build
sdcc-sdcc -V --verbose --iram-size 256 --xram-size 1024 --code-size 60000 --int-long-reent --float-reent --no-xinit-opt --peep-return --peep-asm --opt-code-speed --out-fmt-ihx --model-medium -I ../include -c ../src/sha256.c
sdcc-sdar -rc sha256.lib sha256.rel

sdcc-sdcc --iram-size $IRAM_SIZE \
           --xram-size $XRAM_SIZE \
           --code-size $CODE_SIZE \
           --model-medium \
           $SDCC_OPTS \
           -l sha256.lib ../src/tx.c           
mv tx.ihx ../bin/tx.hex

sdcc-sdcc --iram-size $IRAM_SIZE \
           --xram-size $XRAM_SIZE \
           --code-size $CODE_SIZE \
           --model-medium \
           $SDCC_OPTS \
           -l sha256.lib ../src/rx.c
mv rx.ihx ../bin/rx.hex
