{
  "targets": [
    {
      "target_name": "imagick",
      "variables": {
        "pkg_config_path": "vendor/ImageMagick/build/lib/pkgconfig",
        "magic_config": "vendor/ImageMagick/Magick++/bin/Magick++-config"
      },
      "sources": [
        "src/imagick.cc",
        "src/utils.cc",
        "src/image.cc",
        "src/worker.cc"
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        "vendor/ImageMagick/build/include/ImageMagick-7"
      ],
      "libraries": [
        "-L../vendor/ImageMagick/build/lib -lMagick++-7.Q16HDRI -lMagickWand-7.Q16HDRI -lMagickCore-7.Q16HDRI",
        "-Wl,-rpath='$${ORIGIN}/../../vendor/ImageMagick/build/lib'"
      ],
      "cflags_cc": [
        "-DDEBUG",
        "-fexceptions",
        "-fopenmp -DMAGICKCORE_HDRI_ENABLE=1 -DMAGICKCORE_QUANTUM_DEPTH=16 -fopenmp -DMAGICKCORE_HDRI_ENABLE=1 -DMAGICKCORE_QUANTUM_DEPTH=16 -fopenmp -DMAGICKCORE_HDRI_ENABLE=1 -DMAGICKCORE_QUANTUM_DEPTH=16"
      ]
    }
  ]
}
