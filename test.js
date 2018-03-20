'use strict';

const fs = require('fs');
const readline = require('readline');
const { Image } = require('./lib');

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

    const imEmpty = new Image('300x300', 'red');
    await imEmpty.write('output/empty.jpg');
    return;


    const buffer = await readFilePromise('input/test.jpg');
    const baseImage = new Image();
    await baseImage.read(buffer);

    const im1 = new Image(baseImage);
    await im1.rotate();
    await im1.resize(600, 600, 'areafill');
    await im1.write('output/test-autorotate-resize.jpg');

    // await (new Image(baseImage)).process({
    //     autorotate: true,
    //     strip: true,
    //     output: {
    //         file: `output/test-autorotate.jpg`,
    //     },
    // });
    // await (new Image(baseImage)).process({
    //     autorotate: true,
    //     // resize: "3024",
    //     resize: "300x300",
    //     extent: {
    //         size: "300x300",
    //         color: "red",
    //     },
    //     strip: true,
    //     output: {
    //         file: `output/test-autorotate-resize.jpg`,
    //     },
    // });
    // await (new Image(baseImage)).process({
    //     autorotate: true,
    //     crop: "3024x1701^",
    //     strip: true,
    //     output: {
    //         file: `output/test-autorotate-crop.jpg`,
    //     },
    // });

    dumpMemoryUsage();
    return;

    let index = 0;
    promises = []
    do {
        const output = `output/test-${++index}.jpg`;
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
    return;

    const rl = readline.createInterface({
        input: process.stdin,
        output: process.stdout,
        terminal: false,
    });
    rl.on('line', async (line) => {
        // await test(baseImage.copy(), buffer, ++i).then(dumpMemoryUsage);
    });
})();
