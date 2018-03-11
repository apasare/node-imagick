const fs = require('fs');
const os = require('os');
const readline = require('readline');
const imagick = require('bindings')('imagick.node');


const Queue = {
    concurrency: os.cpus().length,
    queueListner: function(counter) {

    },
};

class Image {
    constructor(image) {
        if (image instanceof Image) {
            this._image = image.copyImageMagick();
        } else {
            this._image = new imagick.Image();
        }
    }

    static concurrency(concurrency) {
       if (typeof(concurrency) === 'number' && concurrency > 0) {
            Queue.concurrency = concurrency;
       }
       return Queue.concurrency;
    }

    process(options, callback) {
        if (typeof(callback) !== 'function') {
            return new Promise((resolve, reject) => {
                this._image.process(options, (err, data) => {
                    if (err) return reject(err);
                    resolve(data);
                });
            });
        } else {
            return this._image.process(options, callback);
        }
    }

    copyImageMagick() {
        return this._image.copy();
    }

    size(width, height) {
        return this._image.size(width, height);
    }
}






function dumpMemoryUsage(data) {
    global.gc();
    console.log(process.memoryUsage());
}

function readFilePromise(filePath) {
    return new Promise((resolve, reject) => {
        fs.readFile(filePath, (err, data) => {
            if (err) return reject(err);
            resolve(data);
        });
    });
}

(async () => {
    console.log(`PID ${process.pid}`);
    const buffer = await readFilePromise('input/test.jpg');
    const baseImage = new Image();
    await baseImage.process({
        input: {buffer},
    });

    await (new Image(baseImage)).process({
        autorotate: true,
        strip: true,
        output: {
            file: `output/test-autorotate.jpg`,
        },
    });
    await (new Image(baseImage)).process({
        autorotate: true,
        // resize: "3024",
        resize: "100x100^",
        strip: true,
        output: {
            file: `output/test-autorotate-resize.jpg`,
        },
    });
    await (new Image(baseImage)).process({
        autorotate: true,
        crop: "3024x1701^",
        strip: true,
        output: {
            file: `output/test-autorotate-crop.jpg`,
        },
    });

    return;
    dumpMemoryUsage();

    let index = 0;
    promises = []
    do {
        let output = `output/test-${++index}.jpg`;
        promises.push((new Image(baseImage)).process({
            output: {
                file: output,
            },
            autorotate: true,
            strip: true,
        }));
    } while (index < 10);
    Promise.all(promises).then((files) => {
        console.log(files);
    }).then(dumpMemoryUsage);

    const rl = readline.createInterface({
        input: process.stdin,
        output: process.stdout,
        terminal: false,
    });
    rl.on('line', async (line) => {
        // await test(baseImage.copy(), buffer, ++i).then(dumpMemoryUsage);
    });
})();
