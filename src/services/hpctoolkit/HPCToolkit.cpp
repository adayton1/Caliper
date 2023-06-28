// Copyright (c) 2015-2022, Lawrence Livermore National Security, LLC.
// See top-level LICENSE file for details.

// Caliper HPCToolkit profiler annotation binding

#include "caliper/AnnotationBinding.h"
#include "caliper/Caliper.h"
#include "caliper/common/Attribute.h"
#include "caliper/common/Variant.h"

#include <hpctoolkit.h>


namespace cali
{

class HPCToolkitBinding : public AnnotationBinding
{
public:
    const char* service_tag() const { return "hpctoolkit"; }

    void on_begin(Caliper*, Channel*, const Attribute&, const Variant&) {
        hpctoolkit_sampling_start();
    }

    void on_end(Caliper*, Channel*, const Attribute&, const Variant&) {
        hpctoolkit_sampling_stop();
    }
};

CaliperService hpctoolkit_service { "hpctoolkit", &AnnotationBinding::make_binding<HPCToolkitBinding> };

} // namespace cali
