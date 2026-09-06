/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  variant_setget.h                                                      */
/**************************************************************************/

#pragma once

#include "core/string/string_name.h"
#include "core/variant/variant.h"

namespace Beyota {

class VariantSetGet {
public:
    static Variant get_indexed(const Variant &p_variant, usize p_idx);
    static bool set_indexed(Variant &r_variant, usize p_idx, const Variant &p_val);

    static Variant get_named(const Variant &p_variant, const StringName &p_name);
    static bool set_named(Variant &r_variant, const StringName &p_name, const Variant &p_val);
};

} // namespace Beyota
