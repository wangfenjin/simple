const { task, logger } = require("just-scripts");
const download = require("download");
const path = require("path");
task("install", () => {
  return new Promise((resolve, reject) => {
    const localPath = path.join(__dirname, "lib");
    var platform = process.env.npm_config_target_platform || process.platform;
    var arch = process.env.npm_config_target_arch || process.arch;

    let downloadUrl = `https://github.com/wangfenjin/simple/releases/latest/download/libsimple-linux-ubuntu-18.04.zip`;
    if (platform === "darwin") {
      platform = "osx";
      downloadUrl = `https://github.com/wangfenjin/simple/releases/latest/download/libsimple-osx-x64.zip`;
    } else if (platform === "win32") {
      platform = "windows";
      if (arch === "x64") {
        downloadUrl = `https://github.com/wangfenjin/simple/releases/latest/download/libsimple-windows-x64.zip`;
      } else {
        downloadUrl = `https://github.com/wangfenjin/simple/releases/latest/download/libsimple-windows-x86.zip`;
      }
    }

    logger.info(`[install] Target platform: -${platform}-`);
    logger.info(`[install] Target arch: ${arch}`);
    logger.info(`[install] Download prebuilt binaries from ${downloadUrl}`);

    
    download(downloadUrl, localPath, {
      extract: true,
      strip: 1,
    })
      .then(() => {
        resolve();
      })
      .catch((err) => {
        logger.warn(
          `[install] Failed to download package from: ${downloadUrl}, err: ${err}`
        );
        reject();
      });
  });
});
