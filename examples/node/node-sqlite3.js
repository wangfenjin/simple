var path = require("path");
var sqlite3 = require('sqlite3').verbose();
var db = new sqlite3.Database(':memory:');

const process = require( 'process' );

const argv = key => {
    // Return true if the key exists and a value is defined
    if ( process.argv.includes( `--${ key }` ) ) return true;
    const value = process.argv.find( element => element.startsWith( `--${ key }=` ) );
    // Return null if the key does not exist and a value is not defined
    if ( !value ) return null;
    return value.replace( `--${ key }=` , '' );
}

db.serialize(function() {
    var ext_path = path.resolve("./lib/");
    if (argv('ext_path')) {
        ext_path = argv('ext_path');
    }
    var dict_path = path.join(ext_path, "dict");
    if (argv('dict_path')) {
        dict_path = argv('dict_path');
    }
    console.log("extension path: " + ext_path + ", dict path: " + dict_path);
    // load extension
    var platform = process.env.npm_config_target_platform || process.platform
    if (platform === 'win32') {
      db.loadExtension(path.join(ext_path, "simple"));
    } else {
      db.loadExtension(path.join(ext_path, "libsimple"));
    }
    // set the jieba dict file path
    db.run("select jieba_dict(?)", dict_path);
    // create table
    db.run("CREATE VIRTUAL TABLE t1 USING fts5(x, tokenize = 'simple')");
    // insert some data
    db.run("insert into t1(x) values ('周杰伦 Jay Chou:我已分不清，你是友情还是错过的爱情'), ('周杰伦 Jay Chou:最美的不是下雨天，是曾与你躲过雨的屋檐'), ('I love China! 我爱中国！我是中华人民共和国公民！'), ('@English &special _characters.\"''bacon-&and''-eggs%')");

    // with match 周杰伦
    db.each("select rowid as id, simple_highlight(t1, 0, '[', ']') as info from t1 where x match simple_query('zjl')", function(err, row) {
        console.log(row.id + ": " + row.info);
    });
    // will match 中国 and 中华人民共和国
    db.each("select rowid as id, simple_highlight(t1, 0, '[', ']') as info from t1 where x match simple_query('中国')", function(err, row) {
        console.log(row.id + ": " + row.info);
    });
    // will match 中国 but not 中华人民共和国
    db.each("select rowid as id, simple_highlight(t1, 0, '[', ']') as info from t1 where x match jieba_query('中国')", function(err, row) {
        console.log(row.id + ": " + row.info);
    });
});

db.close();
