module sllist

one sig SLList {
 header: lone Node
}

sig Node {
  link: lone Node
}

pred RepOk(l: SLList) {
  all n: l.header.*link | n !in n.^link
}

fact Reachability { SLList.header.*link = Node }

--run RepOk for exactly 6 Node
