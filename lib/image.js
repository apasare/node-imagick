const os = require('os');
const imagick = require('bindings')('imagick.node');

const Queue = {
    concurrency: os.cpus().length,
    queueListner: function(counter) {

    },
};

class Image {
    constructor(image) {
        if (image instanceof Image) {
            this._image = image._image.copy();
        } else {
            switch (arguments.length) {
                case 2:
                    this._image = new imagick.Image(arguments[0], arguments[1]);
                    break;
                case 1:
                    this._image = new imagick.Image(arguments[0]);
                    break;
                default:
                    this._image = new imagick.Image();
                    break;
            }
        }
    }

    static concurrency(concurrency) {
       if (typeof(concurrency) === 'number' && concurrency > 0) {
            Queue.concurrency = concurrency;
       }
       return Queue.concurrency;
    }

    process(options) {
        return new Promise((resolve, reject) => {
            this._image.process(options, (err, data) => {
                if (err) return reject(err);
                resolve(data);
            });
        });
    }

    read(from) {
        const input = {};
        if (from instanceof Buffer) {
            input.buffer = from;
        } else {
            input.file = from;
        }

        return this.process({ input });
    }

    rotate(angle = 0) {
        const options = {};
        if (angle) {
            options.rotate = angle;
        } else {
            options.autorotate = true;
        }

        return this.process(options);
    }

    resize(width, height = 0, type = 'areafit') {
        let geometry = `${width}`;
        if (height > 0) {
            geometry += `x${height}`;
        }
        switch (type) {
            case 'stretch':
                geometry += '!';
                break;
            case 'areafill':
                geometry += '^';
                break;
            case 'limitpixels':
                geometry += '@';
                break;
        }

        return this.process({ resize: geometry });
    }

    crop(width, height, xOffset = 0, yOffset = 0) {
        let geometry = `${width}x${height}`;
        if (xOffset) {
            geometry += `+${xOffset}`;
            if (yOffset) {
                yOffset += `+${yOffset}`;
            }
        }

        return this.process({ crop: geometry });
    }

    write(to = null) {
        const output = {};
        if (to) {
            output.file = to;
        } else {
            output.buffer = true;
        }

        return this.process({ output });
    }

    size(width, height) {
        return this._image.size(width, height);
    }
}

module.exports = Image;
