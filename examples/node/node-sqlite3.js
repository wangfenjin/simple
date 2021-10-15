var sqlite3 = require('sqlite3').verbose();
var db = new sqlite3.Database(':memory:');

db.serialize(function() {
    db.loadExtension("./lib/libsimple");
    db.run("CREATE VIRTUAL TABLE t1 USING fts5(x, tokenize = 'simple')");
    db.run("insert into t1(x) values ('周杰伦 Jay Chou:我已分不清，你是友情还是错过的爱情'), ('周杰伦 Jay Chou:最美的不是下雨天，是曾与你躲过雨的屋檐'), ('I love China! 我爱中国!'), ('@English &special _characters.\"''bacon-&and''-eggs%')");

    db.each("select rowid as id, simple_highlight(t1, 0, '[', ']') as info from t1 where x match simple_query('zjl')", function(err, row) {
        console.log(row.id + ": " + row.info);
    });
});

db.close();
