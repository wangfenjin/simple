var path = require("path");
const process = require( 'process' );
const argv = key => {
    // Return true if the key exists and a value is defined
    if ( process.argv.includes( `--${ key }` ) ) return true;
    const value = process.argv.find( element => element.startsWith( `--${ key }=` ) );
    // Return null if the key does not exist and a value is not defined
    if ( !value ) return null;
    return value.replace( `--${ key }=` , '' );
}

var ext_path = path.resolve("./lib/");
if (argv('ext_path')) {
    ext_path = argv('ext_path');
}
var dict_path = path.join(ext_path, "dict");
if (argv('dict_path')) {
    dict_path = argv('dict_path');
}
console.log("extension path: " + ext_path + ", dict path: " + dict_path);

const db = require('better-sqlite3')(':memory:', { verbose: console.log });
db.loadExtension(path.join(ext_path, 'libsimple'));
// test simple_query
const row = db.prepare('select simple_query(\'pinyin\') as query').get();
console.log(row.query);

// set the jieba dict file path
db.prepare("select jieba_dict(?)").run(dict_path);
