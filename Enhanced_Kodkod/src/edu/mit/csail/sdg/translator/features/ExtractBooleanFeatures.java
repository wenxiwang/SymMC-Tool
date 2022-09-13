package edu.mit.csail.sdg.translator.features;

import java.io.PrintWriter;
import java.io.StringWriter;
import kodkod.ast.Formula;
import kodkod.engine.bool.BooleanFormula;
import kodkod.engine.config.Options;
import kodkod.engine.fol2sat.Translator;
import kodkod.instance.Bounds;

public class ExtractBooleanFeatures {
  public static String convert(Formula formula, Bounds bounds, Options options) {
    StringWriter string = new StringWriter();
    PrintWriter file = new PrintWriter(string);
    new ExtractBooleanFeatures(file, formula, bounds, options);
    if (file.checkError()) {
      return ""; // shouldn't happen
    } else {
      return string.toString();
    }
  }

  private ExtractBooleanFeatures(PrintWriter pw, Formula formula, Bounds bounds, Options options) {
    BooleanFormula booleanFormula = Translator.translateToBoolean(formula, bounds, options);
    // Clean the features before collecting
    BooleanFeatureExtractor.booleanFeatureStore.reset();
    // If the boolean formula is null, then it means the formula is trivially true or false.
    // So we do not need to collect any features.
    if (booleanFormula != null) {
      booleanFormula.accept(new BooleanFeatureExtractor(), null);
    }
    pw.printf(BooleanFeatureExtractor.booleanFeatureStore.toCsvRow());
  }
}
