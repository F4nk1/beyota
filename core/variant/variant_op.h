/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  variant_op.h                                                          */
/**************************************************************************/

#pragma once

#include "core/variant/variant.h"

namespace Beyota {

class VariantOp {
public:
    static void evaluate(Variant::Operator p_op, const Variant &p_left, const Variant &p_right, Variant &r_ret, bool &r_valid);
};

} // namespace Beyota
