package edu.mit.csail.sdg.translator.features;

import kodkod.engine.bool.BooleanFormula;
import kodkod.engine.bool.BooleanVariable;
import kodkod.engine.bool.BooleanVisitor;
import kodkod.engine.bool.ITEGate;
import kodkod.engine.bool.MultiGate;
import kodkod.engine.bool.NotGate;

public class BooleanFeatureExtractor implements BooleanVisitor<Integer, Object> {

  public static BooleanFeatureStore booleanFeatureStore = new BooleanFeatureStore();

  private int max(int a, int b) {
    return (a >= b) ? a : b;
  }

  private int max(int a, int b, int c) {
    return (a >= b) ? (a >= c ? a : c) : (b >= c ? b : c);
  }

  @Override
  public Integer visit(MultiGate multigate, Object arg) {
    booleanFeatureStore.multiGateNum++;
    booleanFeatureStore.totalNodeNum++;
    int first = 0, second = 0;
    for (BooleanFormula input : multigate) {
      int iH = input.accept(this, arg);
      if (iH > first) {
        second = first;
        first = iH;
      } else if (iH > second) {
        second = iH;
      }
    }
    booleanFeatureStore.nodeHeight = max(booleanFeatureStore.nodeHeight, first + 1);
    booleanFeatureStore.nodeDiameter = max(booleanFeatureStore.nodeDiameter, first + second + 1);
    return first + 1;
  }

  @Override
  public Integer visit(ITEGate ite, Object arg) {
    booleanFeatureStore.iteGateNum++;
    booleanFeatureStore.totalNodeNum++;
    int i0H = ite.input(0).accept(this, arg);
    int i1H = ite.input(1).accept(this, arg);
    int i2H = ite.input(2).accept(this, arg);
    int res = max(i0H, i1H, i2H) + 1;
    booleanFeatureStore.nodeHeight = max(booleanFeatureStore.nodeHeight, res);
    booleanFeatureStore.nodeDiameter = max(booleanFeatureStore.nodeDiameter, max(i0H + i1H, i0H + i2H, i1H + i2H) + 1);
    return res;
  }

  @Override
  public Integer visit(NotGate negation, Object arg) {
    booleanFeatureStore.notGateNum++;
    booleanFeatureStore.totalNodeNum++;
    int iH = negation.input(0).accept(this, arg);
    booleanFeatureStore.nodeHeight = max(booleanFeatureStore.nodeHeight, iH + 1);
    booleanFeatureStore.nodeDiameter = max(booleanFeatureStore.nodeDiameter, iH + 1);
    return iH + 1;
  }

  @Override
  public Integer visit(BooleanVariable variable, Object arg) {
    booleanFeatureStore.booleanVarNum++;
    booleanFeatureStore.totalNodeNum++;
    booleanFeatureStore.nodeHeight = max(booleanFeatureStore.nodeHeight, 1);
    booleanFeatureStore.nodeDiameter = max(booleanFeatureStore.nodeDiameter, 1);
    return 1;
  }
}
