(progn
	(setq S1 '(RM F G H J (J G D)))
	(setq S2 '(2 3 4 5 6 (7 8)))
	(setq S3 '(U N Y L O O P))
	
	(defun task (a b c) 
		(cons 
			(fourth a)
				(cons
					(sixth b) 
					(second c)
				)
		)
	)
	
	(print (task s1 s2 s3))
	
)
