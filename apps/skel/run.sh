CUR_DIR=$(readlink -e $(dirname $0))
cd $CUR_DIR

BTYPE="konic80"
if [ -e /mppa/board0/type ]; then
	BTYPE=$(cat /mppa/board0/type)
fi
case "$BTYPE" in
	"konic80")
		FIRMWARE=firmware-konic80.kelf
		;;
	"ab01")
		FIRMWARE=firmware-ab01.kelf
		;;
	*)
		echo "Unsupported board"
		exit 1
		;;
esac

if [ $(find . -name "*.mpk" | wc -l) != "1" ]; then
	echo "ERROR: No or multiple mpk available in the directory"
fi
MPK=$(find . -name "*.mpk")
exec k1-jtag-runner --progress --multibinary=$MPK --exec-multibin=IODDR0:${FIRMWARE} --exec-multibin=IODDR1:${FIRMWARE} -- "$@"
