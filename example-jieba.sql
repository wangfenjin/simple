select '使用jieba分词：';
-- will match
select '    ', simple_highlight(t1, 0, '[', ']') from t1 where x match simple_query('国中woai');
select '    ', simple_highlight(t1, 0, '[', ']') from t1 where x match jieba_query('中国woai');
select '    ', simple_highlight(t1, 0, '[', ']') from t1 where x match jieba_query('中国woai', 0);
-- will not match, in jieba_query, the order matters
select '    ', simple_highlight(t1, 0, '[', ']') from t1 where x match jieba_query('国中woai');
