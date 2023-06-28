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
    unsigned int m_stack_size { 0 };

public:
    const char* service_tag() const override { return "hpctoolkit"; }

    void on_begin(Caliper*, Channel*, const Attribute&, const Variant&) override {
        if (attr.is_nested()) {
            ++m_stack_size;
            hpctoolkit_sampling_start();
        }
        else {

        }
    }

    void on_end(Caliper*, Channel*, const Attribute&, const Variant&) override {
        if (attr.is_nested()) {
            hpctoolkit_sampling_stop();
            --m_stack_size;

            if (m_stack_size < 0) {
                ++m_num_stack_errors;

            }
        }
        else {

        }
    }
};

CaliperService hpctoolkit_service { "hpctoolkit", &AnnotationBinding::make_binding<HPCToolkitBinding> };

} // namespace cali
