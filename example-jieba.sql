select '';
select '';
select '--------------------------------------------------------------------------------';
select '使用jieba分词：';
-- will match
select '    ', simple_highlight(t1, 0, '[', ']') from t1 where x match simple_query('国中woai');
select '    ', simple_highlight(t1, 0, '[', ']') from t1 where x match jieba_query('中国woai');
-- will not match, in jieba_query, the order matters
select '    !!!! should not match', simple_highlight(t1, 0, '[', ']') from t1 where x match simple_query('国中woai', 0);
select '    !!!! should not match', simple_highlight(t1, 0, '[', ']') from t1 where x match jieba_query('国中woai');
select '    !!!! should not match', simple_highlight(t1, 0, '[', ']') from t1 where x match jieba_query('中国woai', 0);
select '    !!!! should not match', simple_highlight(t1, 0, '[', ']') from t1 where x match jieba_query('aiwo', 0);
