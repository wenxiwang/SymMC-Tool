sig Queen {}

one sig Board {
    state: Queen ->Int ->Int }

fact {
    all q: Queen | one q.(Board.state)
    all x: Queen.(Board.state).Int | ValidIndex[x]
    all y: Int.(Queen.(Board.state)) | ValidIndex[y]
    all disj q, r: Queen | q.(Board.state) != r.(Board.state) }

pred ValidIndex[x: Int] { x.gte[0] and x.lte[MaxIndex[]] }

fun MaxIndex: Int { 3 } -- SET it to (k - 1) for board size k x k

fun X[q: Queen]: Int { (q.(Board.state)).Int }

fun Y[q: Queen]: Int { Int.(q.(Board.state)) }

fun Abs[x: Int]: Int { x.lt[0] implies negate[x] else x }

pred SameRow[q, r: Queen] { X[q] = X[r] }

pred SameColumn[q, r: Queen] { Y[q] = Y[r] }

pred SameDiagonal[q, r: Queen] { Abs[X[q].minus[X[r]]] =
Abs[Y[q].minus[Y[r]]] }

pred NQueensProblem {
    all disj q, r: Queen | !SameRow[q, r] and !SameColumn[q, r] and
!SameDiagonal[q, r] }

run NQueensProblem for 5 int, exactly 3 Queen

--fact Block { no Queen.(Board.state) & (0 -0 + 0 -2) }
