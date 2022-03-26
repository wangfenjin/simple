const { task, logger } = require('just-scripts')
const download = require('download')
const path = require('path')

task('install', () => {
  return new Promise((resolve, reject) => {
    const localPath = path.join(__dirname, 'lib')
    var platform = process.env.npm_config_target_platform || process.platform
    logger.info(`[install] Target platform: ${platform}`)
    if (platform === 'darwin') {
      platform = 'osx';
    } else if (platform === 'win32') {
      platform = 'windows';
    }
    var arch = process.env.npm_config_target_arch || process.arch
    logger.info(`[install] Target arch: ${arch}`)
    const downloadUrl = `https://github.com/wangfenjin/simple/releases/download/v0.2.0-alpha/libsimple-linux-ubuntu-18.04.zip`
    logger.info(`[install] Download prebuilt binaries from ${downloadUrl}`)
    download(downloadUrl, localPath, {
        extract: true, strip: 1
    }).then(() => {
      resolve()
    }).catch(err => {
      logger.warn(`[install] Failed to download package from: ${downloadUrl}, err: ${err}`)
      reject()
    })
  })
})
