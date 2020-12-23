-- Examples to use simple tokenizer

-- load so file
.load libsimple

-- set tokenize to simple
CREATE VIRTUAL TABLE t1 USING fts5(x, tokenize = 'simple');

-- add some values into the table
insert into t1(x) values ('周杰伦 Jay Chou:最美的不是下雨天，是曾与你躲过雨的屋檐'),
                      ('I love China! 我爱中国!') ;
select '所有数据:';
select '    ', * from t1;

select '搜索 杰伦:';
select '    ', simple_highlight(t1, 0, '[', ']') from t1 where x match simple_query('杰伦');

select '搜索 zhoujiel:';
select '    ', simple_highlight(t1, 0, '[', ']') from t1 where x match simple_query('zhoujiel');

select '搜索 zjl:';
select '    ', simple_highlight(t1, 0, '[', ']') from t1 where x match simple_query('zjl');

select '搜索 ZHOUJi:';
select '    ', simple_highlight(t1, 0, '[', ']') from t1 where x match simple_query('ZHOUJi');

select '搜索 love zg:';
select '    ', simple_highlight(t1, 0, '[', ']') from t1 where x match simple_query('love zg');
