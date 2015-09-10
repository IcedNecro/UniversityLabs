(progn
	(defun task (arg) 
		(cond
			( (eq (numberp arg) Nil) (listp arg) )
			( (= (rem arg 2) 0) (* arg arg) )
			( (= (rem arg 2) 1) (log arg) )
		)
	)
	(print (string "Input your value:"))
	(print (task (read)))
)
