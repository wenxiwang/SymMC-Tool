/*
 * Kodkod -- Copyright (c) 2005-present, Emina Torlak
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
package kodkod.ast;

import kodkod.ast.operator.ExprOperator;
import kodkod.ast.visitor.ReturnVisitor;
import kodkod.ast.visitor.VoidVisitor;

/**
 * A relational {@link kodkod.ast.Expression expression} with two children.
 *
 * @specfield left: Expression
 * @specfield right: Expression
 * @specfield op: ExprOperator
 * @specfield op.binary()
 * @invariant children = 0->left + 1->right
 * @author Emina Torlak
 */
public final class BinaryExpression extends Expression {

    private final ExprOperator op;
    private final Expression   left;
    private final Expression   right;
    private final int          arity;

    /**
     * Constructs a new binary expression: left op right
     *
     * @ensures this.left' = left && this.right' = right && this.op' = op
     * @throws NullPointerException left = null || right = null || op = null
     * @throws IllegalArgumentException left and right cannot be combined with the
     *             specified operator.
     */
    BinaryExpression(final Expression left, final ExprOperator op, final Expression right) {
        switch (op) {
            case UNION :
            	this.arity = left.arity();
                if (arity != right.arity())
                    throw new IllegalArgumentException("Incompatible arities: " + left + " and " + right);
                this.bounds = (left.bounds + right.bounds + Math.max(left.bounds, right.bounds))/2;
                break;
            case INTERSECTION :
            	this.arity = left.arity();
                if (arity != right.arity())
                    throw new IllegalArgumentException("Incompatible arities: " + left + " and " + right);
                this.bounds = (Math.min(left.bounds, right.bounds) + Math.max(left.bounds, right.bounds))/2;
                break;
            case DIFFERENCE :
            	this.arity = left.arity();
                if (arity != right.arity())
                    throw new IllegalArgumentException("Incompatible arities: " + left + " and " + right);
                this.bounds = left.bounds - (Math.min(left.bounds, right.bounds) + Math.max(left.bounds, right.bounds))/2;
                break;
            case OVERRIDE :
            	this.arity = left.arity();
                if (arity != right.arity())
                    throw new IllegalArgumentException("Incompatible arities: " + left + " and " + right);
                
                break;
            case JOIN :
                this.arity = left.arity() + right.arity() - 2;
                if (arity < 1)
                    throw new IllegalArgumentException("Incompatible arities: " + left + " and " + right);
                this.bounds = right.bounds;
                break;
            case PRODUCT :
                this.arity = left.arity() + right.arity();
                this.bounds = left.bounds * right.bounds;
                break;
            default :
                throw new IllegalArgumentException("Not a binary operator: " + op);
        }

        this.op = op;
        this.left = left;
        this.right = right;
        
    }

    /**
     * Returns the arity of this binary expression.
     *
     * @return this.arity
     * @see kodkod.ast.Expression#arity()
     */
    @Override
    public int arity() {
        return arity;
    }

    /**
     * Returns this.op.
     *
     * @return this.op
     */
    public ExprOperator op() {
        return op;
    }

    /**
     * Returns the left child of this.
     *
     * @return this.left
     */
    public Expression left() {
        return left;
    }

    /**
     * Returns the right child of this.
     *
     * @return this.right
     */
    public Expression right() {
        return right;
    }

    /**
     * {@inheritDoc}
     *
     * @see kodkod.ast.Expression#accept(kodkod.ast.visitor.ReturnVisitor)
     */
    @Override
    public <E, F, D, I> E accept(ReturnVisitor<E,F,D,I> visitor) {
        return visitor.visit(this);
    }

    /**
     * {@inheritDoc}
     *
     * @see kodkod.ast.Node#accept(kodkod.ast.visitor.VoidVisitor)
     */
    @Override
    public void accept(VoidVisitor visitor) {
        visitor.visit(this);
    }

    /**
     * {@inheritDoc}
     *
     * @see kodkod.ast.Node#toString()
     */
    @Override
    public String toString() {
        return "(" + left + " " + op + " " + right + ")";
    }
}
