module.exports = {
  branches: ["master", "semver"],
  plugins: [
    "@semantic-release/commit-analyzer",
    "@semantic-release/release-notes-generator",
    "@semantic-release/changelog",
    [
      "@semantic-release/exec",
      {
        
        prepareCmd: "V=${nextRelease.version} && " +
                    "MAJOR=$(echo $V | cut -d. -f1) && " +
                    "MINOR=$(echo $V | cut -d. -f2) && " +
                    "PATCH=$(echo $V | cut -d. -f3) && " +
                    "echo \"#ifndef VERSION_H\\n#define VERSION_H\\n\\n#define FW_VERSION \\\"$V\\\"\\n#define FW_VERSION_MAJOR $MAJOR\\n#define FW_VERSION_MINOR $MINOR\\n#define FW_VERSION_PATCH $PATCH\\n\\n#endif\" > bms/Core/Inc/version.h && " +
                    "cmake -S bms -B bms/build -G Ninja -DCMAKE_TOOLCHAIN_FILE=cmake/gcc-arm-none-eabi.cmake && " +
                    "cmake --build bms/build"
      }
    ],
    [
      "@semantic-release/github",
      {
        "assets": [
          {"path": "bms/build/*.elf", "label": "BMS-FW-${nextRelease.version}.elf"}
        ]
      }
    ],
    [
      "@semantic-release/git",
      {
        "assets": ["bms/Core/Inc/version.h", "CHANGELOG.md"],
        "message": "chore(release): ${nextRelease.version} [skip ci]"
      }
    ]
  ]
};