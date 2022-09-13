module bt

one sig BT {
  root: lone Node
}

sig Node {
  left, right: lone Node
}

pred Acyclic(t: BT) {
  all n: t.root.*(left + right) {
    n !in n.^(left + right)
    lone n.~(left + right)
    no n.left & n.right
  }
}

pred RepOk(t: BT) {
  Acyclic[t]
}

fact Reachability { BT.root.*(left + right) = Node }

--run RepOk for exactly 4 Node
