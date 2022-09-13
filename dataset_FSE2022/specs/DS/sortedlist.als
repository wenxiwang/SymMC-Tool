module sortedlist

one sig SortedList {
 header: lone Node
}

sig Node {
  elem: Int,
  link: lone Node
}

pred Acyclic(l: SortedList) {
  all n: l.header.*link | n !in n.^link
}

pred Sorted(l: SortedList) {
  all n: l.header.*link |
    some n.link implies n.elem.lt[n.link.elem] or n.elem = n.link.elem
}

pred RepOk(l: SortedList) {
  Acyclic[l]
  Sorted[l]
}

fact Reachability { SortedList.header.*link = Node }

fact ConsecutiveElemsFrom0 {
  0 in Node.elem
  all n: Node | n.elem != 0 implies some m: Node | m.elem = prev[n.elem]
}

--run RepOk for exactly 6 Node

