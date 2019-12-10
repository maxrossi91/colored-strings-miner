SELECT SKIPPING_NO_BOOST.SEED,SKIPPING_NO_BOOST. N_PROPERTIES,SKIPPING_NO_BOOST.N,SKIPPING_NO_BOOST.X,SKIPPING_NO_BOOST.C,SKIPPING_NO_BOOST.MS,SKIPPING_TABLE.MS,SKIPPING_NO_BOOST.MS - SKIPPING_TABLE.MS AS MS_DIFF,SKIPPING_NO_BOOST.S,SKIPPING_TABLE.S,SKIPPING_NO_BOOST.S-SKIPPING_TABLE.S AS S_DIFF,SKIPPING_NO_BOOST.S/SKIPPING_TABLE.S AS SPEEDUP,SKIPPING_NO_BOOST.REPETITIONS,SKIPPING_TABLE.REPETITIONS
FROM (
	SELECT N_PROPERTIES,ALGO_TYPE,SEED,N,X,C,AVG(EXECUTION_TIME_MS) AS MS,AVG(EXECUTION_TIME_S) AS S,Count(*) AS REPETITIONS FROM EXECUTION_TIMES
	WHERE ALGO_TYPE="Skipping_no_boost" 
	GROUP BY ALGO_TYPE,N,X,C,SEED
	) AS SKIPPING_NO_BOOST, 
	(
	SELECT N_PROPERTIES,ALGO_TYPE,SEED,N,X,C,AVG(EXECUTION_TIME_MS) AS MS,AVG(EXECUTION_TIME_S) AS S,Count(*) AS REPETITIONS FROM EXECUTION_TIMES
	WHERE ALGO_TYPE="Skipping" 
	GROUP BY ALGO_TYPE,N,X,C,SEED
	) AS SKIPPING_TABLE
WHERE
	SKIPPING_NO_BOOST.N = SKIPPING_TABLE.N AND
	SKIPPING_NO_BOOST.X = SKIPPING_TABLE.X AND
	SKIPPING_NO_BOOST.C = SKIPPING_TABLE.C AND
	SKIPPING_NO_BOOST.SEED = SKIPPING_TABLE.SEED
GROUP BY SKIPPING_NO_BOOST.SEED, SKIPPING_NO_BOOST.N,SKIPPING_NO_BOOST.X,SKIPPING_NO_BOOST.C;