-- Examples to use simple tokenizer

-- load so file
.load libsimple

select '启用拼音分词：';
-- set tokenize to simple
CREATE VIRTUAL TABLE t1 USING fts5(x, tokenize = 'simple');

-- add some values into the table
insert into t1(x) values ('周杰伦 Jay Chou:最美的不是下雨天，是曾与你躲过雨的屋檐'),
                         ('I love China! 我爱中国!'),
                         ('@English &special _characters."''bacon-&and''-eggs%');
select '所有数据:';
select '    ', * from t1;

select '特殊字符:';
select '    ', simple_highlight(t1, 0, '[', ']') from t1 where x match simple_query('@"''._-&%');
select '    ', simple_highlight(t1, 0, '[', ']') from t1 where x match '"''"';

select '搜索 杰伦:';
select '    ', simple_highlight(t1, 0, '[', ']') from t1 where x match simple_query('杰伦');
select '    ', simple_snippet(t1, 0, '[', ']', '...', 1) from t1 where x match simple_query('杰伦');
select '    ', simple_snippet(t1, 0, '[', ']', '...', 2) from t1 where x match simple_query('杰伦');
select '    ', simple_snippet(t1, 0, '[', ']', '...', 3) from t1 where x match simple_query('杰伦');
select '    ', simple_snippet(t1, 0, '[', ']', '...', 4) from t1 where x match simple_query('杰伦');
select '    ', simple_snippet(t1, 0, '[', ']', '...', 5) from t1 where x match simple_query('杰伦');
select '    ', simple_snippet(t1, 0, '[', ']', '...', 10) from t1 where x match simple_query('杰伦');
select '    ', simple_snippet(t1, 0, '[', ']', '...', 20) from t1 where x match simple_query('杰伦');
select '    ', simple_snippet(t1, 0, '[', ']', '...', 100) from t1 where x match simple_query('杰伦');
select '    ', simple_highlight_pos(t1, 0) from t1 where x match simple_query('杰伦');

select '搜索 雨天:';
select '    ', simple_snippet(t1, 0, '[', ']', '...', 10) from t1 where x match simple_query('雨天');

select '搜索 zhoujiel:';
select '    ', simple_highlight(t1, 0, '[', ']') from t1 where x match simple_query('zhoujiel');
select '    ', simple_highlight_pos(t1, 0) from t1 where x match simple_query('zhoujiel');

select '搜索 zhoujie:';
select '    ', simple_highlight(t1, 0, '[', ']') from t1 where x match simple_query('zhoujie');
-- will not match
select '    !!!!! should not match', simple_highlight_pos(t1, 0) from t1 where x match simple_query('jiezhou');


select '搜索 zjl:';
select '    ', simple_highlight(t1, 0, '[', ']') from t1 where x match simple_query('zjl');
select '    ', simple_highlight_pos(t1, 0) from t1 where x match simple_query('zjl');

select '搜索 ZHOUJi:';
select '    ', simple_highlight(t1, 0, '[', ']') from t1 where x match simple_query('ZHOUJi');
select '    ', simple_highlight_pos(t1, 0) from t1 where x match simple_query('ZHOUJi');

select '搜索 love zg:';
select '    ', simple_highlight(t1, 0, '[', ']') from t1 where x match simple_query('love zg');
select '    ', simple_highlight_pos(t1, 0) from t1 where x match simple_query('love zg');

select '';
select '';
select '--------------------------------------------------------------------------------';
select '禁用拼音分词：';
-- set tokenize to simple, 0 means disable pinyin
CREATE VIRTUAL TABLE t2 USING fts5(x, tokenize = 'simple 0');

-- add some values into the table
insert into t2(x) values ('周杰伦 Jay Chou:最美的不是下雨天，是曾与你躲过雨的屋檐'),
                      ('I love China! 我爱中国!') ;
select '所有数据:';
select '    ', * from t2;

select '搜索 杰伦:';
-- in simple_query, we accept a second params, '0' means disable pinyin split
select '    ', simple_highlight(t2, 0, '[', ']') from t2 where x match simple_query('杰伦', '0');
select '    ', simple_snippet(t2, 0, '[', ']', '...', 1) from t2 where x match simple_query('杰伦', '0');
select '    ', simple_snippet(t2, 0, '[', ']', '...', 2) from t2 where x match simple_query('杰伦', '0');
select '    ', simple_snippet(t2, 0, '[', ']', '...', 3) from t2 where x match simple_query('杰伦', '0');
select '    ', simple_snippet(t2, 0, '[', ']', '...', 4) from t2 where x match simple_query('杰伦', '0');
select '    ', simple_snippet(t2, 0, '[', ']', '...', 5) from t2 where x match simple_query('杰伦', '0');
select '    ', simple_snippet(t2, 0, '[', ']', '...', 10) from t2 where x match simple_query('杰伦', '0');
select '    ', simple_snippet(t2, 0, '[', ']', '...', 20) from t2 where x match simple_query('杰伦', '0');
select '    ', simple_snippet(t2, 0, '[', ']', '...', 100) from t2 where x match simple_query('杰伦', '0');
select '    ', simple_highlight_pos(t2, 0) from t2 where x match simple_query('杰伦', '0');

select '搜索 雨天:';
select '    ', simple_snippet(t2, 0, '[', ']', '...', 10) from t2 where x match simple_query('雨天', '0');

select '搜索 zhoujiel:';
select '    ', simple_highlight(t2, 0, '[', ']') from t2 where x match simple_query('zhoujiel', '0');
select '    ', simple_highlight_pos(t2, 0) from t2 where x match simple_query('zhoujiel', '0');
select '    !!!!! should not match', simple_highlight_pos(t1, 0) from t1 where x match simple_query('jiezhou', '0');

select '搜索 zjl:';
select '    ', simple_highlight(t2, 0, '[', ']') from t2 where x match simple_query('zjl', '0');
select '    ', simple_highlight_pos(t2, 0) from t2 where x match simple_query('zjl', '0');

select '搜索 ZHOUJi:';
select '    ', simple_highlight(t2, 0, '[', ']') from t2 where x match simple_query('ZHOUJi', '0');
select '    ', simple_highlight_pos(t2, 0) from t2 where x match simple_query('ZHOUJi', '0');

select '搜索 love zg:';
select '    ', simple_highlight(t2, 0, '[', ']') from t2 where x match simple_query('love zg', '0');
select '    ', simple_highlight_pos(t2, 0) from t2 where x match simple_query('love zg', '0');
