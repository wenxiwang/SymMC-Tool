package edu.mit.csail.sdg.translator.features;

import java.util.Iterator;
import java.util.Map;

import kodkod.ast.BinaryExpression;
import kodkod.ast.BinaryFormula;
import kodkod.ast.BinaryIntExpression;
import kodkod.ast.ComparisonFormula;
import kodkod.ast.Comprehension;
import kodkod.ast.ConstantExpression;
import kodkod.ast.ConstantFormula;
import kodkod.ast.Decl;
import kodkod.ast.Decls;
import kodkod.ast.ExprToIntCast;
import kodkod.ast.FixFormula;
import kodkod.ast.IfExpression;
import kodkod.ast.IfIntExpression;
import kodkod.ast.IntComparisonFormula;
import kodkod.ast.IntConstant;
import kodkod.ast.IntExpression;
import kodkod.ast.IntToExprCast;
import kodkod.ast.MultiplicityFormula;
import kodkod.ast.NaryExpression;
import kodkod.ast.NaryFormula;
import kodkod.ast.NaryIntExpression;
import kodkod.ast.NotFormula;
import kodkod.ast.ProjectExpression;
import kodkod.ast.QuantifiedFormula;
import kodkod.ast.Relation;
import kodkod.ast.RelationPredicate;
import kodkod.ast.RelationPredicate.Function;
import kodkod.ast.SumExpression;
import kodkod.ast.UnaryExpression;
import kodkod.ast.UnaryIntExpression;
import kodkod.ast.Variable;
import kodkod.ast.operator.ExprOperator;
import kodkod.ast.visitor.ReturnVisitor;
import kodkod.instance.Bounds;

public class FeatureExtractorBound implements ReturnVisitor<Integer,Integer,Integer,Integer> {

	Map<Relation, Integer>  rb_map;
  FeatureExtractorBound(Map<Relation, Integer> rb_map)
  {
	  this.rb_map  = rb_map; 
  }
  public static FeatureStore featureStore = new FeatureStore();

  private int max(int a, int b) {
    return (a >= b) ? a : b;
  }

  private int max(int a, int b, int c) {
    return (a >= b) ? (a >= c ? a : c) : (b >= c ? b : c);
  }

  @Override
  public Integer visit(Relation x) {
    featureStore.relationNum++;
    featureStore.totalNodeNum++;    
    
    featureStore.relationinboundsNum = featureStore.relationinboundsNum + rb_map.get(x);
    x.bounds = rb_map.get(x);
    
    featureStore.nodeHeight = max(featureStore.nodeHeight, 1);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, 1);
    return 1;
  }

  //@Override
  
  @Override
  public Integer visit(IntConstant x) {
    featureStore.intConstNum++;
    featureStore.totalNodeNum++;
    featureStore.nodeHeight = max(featureStore.nodeHeight, 1);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, 1);
    return 1;
  }

  @Override
  public Integer visit(ConstantFormula x) {
    featureStore.constFormulaNum++;
    featureStore.totalNodeNum++;
    featureStore.nodeHeight = max(featureStore.nodeHeight, 1);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, 1);
    return 1;
  }

  @Override
  public Integer visit(Variable x) {
    featureStore.varNum++;
    featureStore.totalNodeNum++;
    featureStore.nodeHeight = max(featureStore.nodeHeight, 1);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, 1);
    return 1;
  }

  @Override
  public Integer visit(ConstantExpression x) {
    featureStore.constExprNum++;
    featureStore.totalNodeNum++;
    featureStore.nodeHeight = max(featureStore.nodeHeight, 1);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, 1);
    return 1;
  }

  @Override
  public Integer visit(NotFormula x) {
    featureStore.notFormulaNum++;
    featureStore.totalNodeNum++;
    int res = 1 + x.formula().accept(this);
    featureStore.nodeHeight = max(featureStore.nodeHeight, res);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, res);
    return res;
  }

  @Override
  public Integer visit(IntToExprCast x) {
    featureStore.intToExprCastNum++;
    featureStore.totalNodeNum++;
    int res = 1 + x.intExpr().accept(this);
    featureStore.nodeHeight = max(featureStore.nodeHeight, res);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, res);
    return res;
  }

  @Override
  public Integer visit(Decl x) {
    featureStore.declNum++;
    featureStore.totalNodeNum++;
    int res = 1 + x.expression().accept(this);
    featureStore.nodeHeight = max(featureStore.nodeHeight, res);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, res);
    return res;
  }

  @Override
  public Integer visit(ExprToIntCast x) {
    featureStore.exprToIntCastNum++;
    featureStore.totalNodeNum++;
    int res = 1 + x.expression().accept(this);
    featureStore.nodeHeight = max(featureStore.nodeHeight, res);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, res);
    return res;
  }

  @Override
  public Integer visit(UnaryExpression x) {
    featureStore.unaryExprNum++;
    featureStore.totalNodeNum++;
    if(x.op() == ExprOperator.REFLEXIVE_CLOSURE || x.op() == ExprOperator.CLOSURE)
    	featureStore.closureNum++;
    	
    int res = 1 + x.expression().accept(this);
    featureStore.nodeHeight = max(featureStore.nodeHeight, res);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, res);
    return res;
  }

  @Override
  public Integer visit(UnaryIntExpression x) {
    featureStore.unaryIntExprNum++;
    featureStore.totalNodeNum++;
    int res = 1 + x.intExpr().accept(this);
    featureStore.nodeHeight = max(featureStore.nodeHeight, res);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, res);
    return res;
  }

  @Override
  public Integer visit(MultiplicityFormula x) {
    featureStore.multiFormulaNum++;
    featureStore.totalNodeNum++;
    int res = 1 + x.expression().accept(this);
    featureStore.nodeHeight = max(featureStore.nodeHeight, res);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, res);
    return res;
  }

  @Override
  public Integer visit(BinaryExpression x) {
    featureStore.binaryExprNum++;
    featureStore.totalNodeNum++;
    int leftH = x.left().accept(this);
    int rightH = x.right().accept(this);
    int res = 1 + max(leftH, rightH);
    featureStore.nodeHeight = max(featureStore.nodeHeight, res);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, leftH + rightH + 1);
    return res;
  }

  @Override
  public Integer visit(ComparisonFormula x) {
    featureStore.compFormulaNum++;
    featureStore.totalNodeNum++;
    int leftH = x.left().accept(this);
    int rightH = x.right().accept(this);
    int res = 1 + max(leftH, rightH);
    featureStore.nodeHeight = max(featureStore.nodeHeight, res);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, leftH + rightH + 1);
    return res;
  }

  @Override
  public Integer visit(BinaryFormula x) {
    featureStore.binaryFormulaNum++;
    featureStore.totalNodeNum++;
    int leftH = x.left().accept(this);
    int rightH = x.right().accept(this);
    int res = 1 + max(leftH, rightH);
    featureStore.nodeHeight = max(featureStore.nodeHeight, res);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, leftH + rightH + 1);
    return res;
  }

  @Override
  public Integer visit(BinaryIntExpression x) {
    featureStore.binaryIntExprNum++;
    featureStore.totalNodeNum++;
    int leftH = x.left().accept(this);
    int rightH = x.right().accept(this);
    int res = 1 + max(leftH, rightH);
    featureStore.nodeHeight = max(featureStore.nodeHeight, res);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, leftH + rightH + 1);
    return res;
  }

  @Override
  public Integer visit(IntComparisonFormula x) {
    featureStore.intCompFormula++;
    featureStore.totalNodeNum++;
    int leftH = x.left().accept(this);
    int rightH = x.right().accept(this);
    int res = 1 + max(leftH, rightH);
    featureStore.nodeHeight = max(featureStore.nodeHeight, res);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, leftH + rightH + 1);
    return res;
  }

  @Override
  public Integer visit(IfExpression x) {
    featureStore.ifExprNum++;
    featureStore.totalNodeNum++;
    int condH = x.condition().accept(this);
    int thenH = x.thenExpr().accept(this);
    int elseH = x.elseExpr().accept(this);
    int res = 1 + max(condH, thenH, elseH);
    featureStore.nodeHeight = max(featureStore.nodeHeight, res);
    int diameter = max(condH + thenH, condH + elseH, thenH + elseH);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, diameter + 1);
    return res;
  }

  @Override
  public Integer visit(IfIntExpression x) {
    featureStore.ifIntExprNum++;
    featureStore.totalNodeNum++;
    int condH = x.condition().accept(this);
    int thenH = x.thenExpr().accept(this);
    int elseH = x.elseExpr().accept(this);
    int res = 1 + max(condH, thenH, elseH);
    featureStore.nodeHeight = max(featureStore.nodeHeight, res);
    int diameter = max(condH + thenH, condH + elseH, thenH + elseH);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, diameter + 1);
    return res;
  }

  @Override
  public Integer visit(SumExpression x) {
    featureStore.sumExprNum++;
    featureStore.totalNodeNum++;
    int declsH = x.decls().accept(this);
    int intExprH = x.intExpr().accept(this);
    int res = 1 + max(declsH, intExprH);
    featureStore.nodeHeight = max(featureStore.nodeHeight, res);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, declsH + intExprH + 1);
    return res;
  }

  @Override
  public Integer visit(QuantifiedFormula x) {
    featureStore.quantFormulaNum++;
    featureStore.totalNodeNum++;
    int declsH = x.decls().accept(this);
    int formulaH = x.formula().accept(this);
    int res = 1 + max(declsH, formulaH);
    featureStore.nodeHeight = max(featureStore.nodeHeight, res);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, declsH + formulaH + 1);
    return res;
  }

  @Override
  public Integer visit(FixFormula x) {
    featureStore.fixFormulaNum++;
    featureStore.totalNodeNum++;
    int condH = x.condition().accept(this);
    int formulaH = x.formula().accept(this);
    int res = 1 + max(condH, formulaH);
    featureStore.nodeHeight = max(featureStore.nodeHeight, res);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, condH + formulaH + 1);
    return res;
  }

  @Override
  public Integer visit(Comprehension x) {
    featureStore.comprehensionNum++;
    featureStore.totalNodeNum++;
    int declsH = x.decls().accept(this);
    int formulaH = x.formula().accept(this);
    int res = 1 + max(declsH, formulaH);
    featureStore.nodeHeight = max(featureStore.nodeHeight, res);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, declsH + formulaH + 1);
    return res;
  }

  @Override
  public Integer visit(Decls x) {
    featureStore.declsNum++;
    featureStore.totalNodeNum++;
    int first = 0, second = 0;
    int n = x.size();
    for (int i = 0; i < n; i++) {
      int iH = x.get(i).accept(this);
      if (iH > first) {
        second = first;
        first = iH;
      } else if (iH > second) {
        second = iH;
      }
    }
    // It seems the original implementation did not use +1 for the height.  Here we modify it.
    featureStore.nodeHeight = max(featureStore.nodeHeight, first + 1);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, first + second + 1);
    return first + 1;
  }

  @Override
  public Integer visit(ProjectExpression x) {
    featureStore.declsNum++;
    featureStore.totalNodeNum++;
    int first = x.expression().accept(this);
    int second = 0;
    for (Iterator<IntExpression> t = x.columns(); t.hasNext();) {
      int tH = t.next().accept(this);
      if (tH > first) {
        second = first;
        first = tH;
      } else if (tH > second) {
        second = tH;
      }
    }
    // It seems the original implementation did not use +1 for the height.  Here we modify it.
    featureStore.nodeHeight = max(featureStore.nodeHeight, first + 1);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, first + second + 1);
    return first + 1;
  }

  @Override
  public Integer visit(RelationPredicate x) {
    featureStore.relationPredNum++;
    featureStore.totalNodeNum++;
    if (x instanceof Function) {
      Function f = ((Function) x);
      int domainH = f.domain().accept(this);
      int rangeH = f.range().accept(this);
      int res = 1 + max(domainH, rangeH);
      featureStore.nodeHeight = max(featureStore.nodeHeight, res);
      featureStore.nodeDiameter = max(featureStore.nodeDiameter, domainH + rangeH + 1);
      return res;
    }
    featureStore.nodeHeight = max(featureStore.nodeHeight, 1);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, 1);
    return 1;
  }

  @Override
  public Integer visit(NaryExpression x) {
    featureStore.naryExprNum++;
    featureStore.totalNodeNum++;
    int first = 0, second = 0;
    for (int m = 0, n = x.size(), i = 0; i < n; i++) {
      m = x.child(i).accept(this);
      if (i == 0) {
        first = m;
        continue;
      }
      if (first < m) {
        second = first;
        first = m;
      } else if (second < m) {
        second = m;
      }
    }
    featureStore.nodeHeight = max(featureStore.nodeHeight, first + 1);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, first + second + 1);
    return first + 1;
  }

  @Override
  public Integer visit(NaryIntExpression x) {
    featureStore.naryIntExprNum++;
    featureStore.totalNodeNum++;
    int first = 0, second = 0;
    for (int m = 0, n = x.size(), i = 0; i < n; i++) {
      m = x.child(i).accept(this);
      if (i == 0) {
        first = m;
        continue;
      }
      if (first < m) {
        second = first;
        first = m;
      } else if (second < m) {
        second = m;
      }
    }
    featureStore.nodeHeight = max(featureStore.nodeHeight, first + 1);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, first + second + 1);
    return first + 1;
  }

  @Override
  public Integer visit(NaryFormula x) {
    featureStore.naryFormula++;
    featureStore.totalNodeNum++;
    int first = 0, second = 0;
    for (int m = 0, n = x.size(), i = 0; i < n; i++) {
      m = x.child(i).accept(this);
      if (i == 0) {
        first = m;
        continue;
      }
      if (first < m) {
        second = first;
        first = m;
      } else if (second < m) {
        second = m;
      }
    }
    featureStore.nodeHeight = max(featureStore.nodeHeight, first + 1);
    featureStore.nodeDiameter = max(featureStore.nodeDiameter, first + second + 1);
    return first + 1;
  }
}
