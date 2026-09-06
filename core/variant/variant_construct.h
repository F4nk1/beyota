/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  variant_construct.h                                                   */
/**************************************************************************/

#pragma once

#include "core/variant/callable.h"
#include "core/variant/variant.h"

namespace Beyota {

class VariantConstruct {
public:
    [[nodiscard]] static Variant construct_default(Variant::Type p_type);
    [[nodiscard]] static Variant construct(Variant::Type p_type, const Variant **p_args, int p_argcount, Callable::CallError &r_error);
};

} // namespace Beyota
