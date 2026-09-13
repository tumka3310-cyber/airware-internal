#include "il2cpp.h"
#include "offsets.h"
#include <android/log.h>
#include <cstring>
#include <dlfcn.h>
#include <link.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "airware", __VA_ARGS__)

namespace il2cpp {

t_domain_get                domain_get                = nullptr;
t_thread_current            thread_current            = nullptr;
t_thread_attach             thread_attach             = nullptr;
t_thread_detach             thread_detach             = nullptr;
t_assembly_get_image        assembly_get_image        = nullptr;
t_class_get_name            class_get_name            = nullptr;
t_class_get_namespace       class_get_namespace       = nullptr;
t_class_get_methods         class_get_methods         = nullptr;
t_class_get_fields          class_get_fields          = nullptr;
t_class_get_parent          class_get_parent          = nullptr;
t_class_get_nested          class_get_nested          = nullptr;
t_class_get_interfaces      class_get_interfaces      = nullptr;
t_class_get_image           class_get_image           = nullptr;
t_class_get_type            class_get_type            = nullptr;
t_class_get_flags           class_get_flags           = nullptr;
t_class_enum_basetype       class_enum_basetype       = nullptr;
t_class_is_enum             class_is_enum             = nullptr;
t_class_is_valuetype        class_is_valuetype        = nullptr;
t_class_is_interface        class_is_interface        = nullptr;
t_method_get_name           method_get_name           = nullptr;
t_method_get_return         method_get_return         = nullptr;
t_method_get_param_count    method_get_param_count    = nullptr;
t_method_get_param          method_get_param          = nullptr;
t_method_is_instance        method_is_instance        = nullptr;
t_field_get_name            field_get_name            = nullptr;
t_field_get_type            field_get_type            = nullptr;
t_field_get_offset          field_get_offset          = nullptr;
t_field_get_flags           field_get_flags           = nullptr;
t_field_static_get_value    field_static_get_value    = nullptr;
t_type_get_name             type_get_name             = nullptr;
t_type_is_byref             type_is_byref             = nullptr;
t_type_get_object           type_get_object           = nullptr;
t_class_from_type           class_from_type           = nullptr;
t_get_all_assemblies        get_all_assemblies        = nullptr;
t_get_assembly_type_handle  get_assembly_type_handle  = nullptr;
t_get_type_info_from_handle get_type_info_from_handle = nullptr;

static uintptr_t g_base = 0;
uintptr_t base() { return g_base; }

static int phdr_cb(struct dl_phdr_info* info, size_t, void* data) {
    if (info->dlpi_name && strstr(info->dlpi_name, "libil2cpp.so")) {
        *reinterpret_cast<uintptr_t*>(data) = info->dlpi_addr;
        return 1;
    }
    return 0;
}

#define BIND(name, type) name = reinterpret_cast<type>(g_base + offsets::il2cpp_##name)

bool init() {
    if (g_base) return true;
    dl_iterate_phdr(phdr_cb, &g_base);
    if (!g_base) { LOGI("airware: libil2cpp.so not found"); return false; }
    LOGI("airware: libil2cpp base = %p", (void*)g_base);

    BIND(domain_get,             t_domain_get);
    BIND(thread_current,         t_thread_current);
    BIND(thread_attach,          t_thread_attach);
    BIND(thread_detach,          t_thread_detach);
    BIND(assembly_get_image,     t_assembly_get_image);
    BIND(class_get_name,         t_class_get_name);
    BIND(class_get_namespace,    t_class_get_namespace);
    BIND(class_get_methods,      t_class_get_methods);
    BIND(class_get_fields,       t_class_get_fields);
    BIND(class_get_parent,       t_class_get_parent);
    BIND(class_get_nested,       t_class_get_nested);
    BIND(class_get_interfaces,   t_class_get_interfaces);
    BIND(class_get_image,        t_class_get_image);
    BIND(class_get_type,         t_class_get_type);
    BIND(class_get_flags,        t_class_get_flags);
    BIND(class_enum_basetype,    t_class_enum_basetype);
    BIND(class_is_enum,          t_class_is_enum);
    BIND(class_is_valuetype,     t_class_is_valuetype);
    BIND(class_is_interface,     t_class_is_interface);
    BIND(method_get_name,        t_method_get_name);
    BIND(method_get_return,      t_method_get_return);
    BIND(method_get_param_count, t_method_get_param_count);
    BIND(method_get_param,       t_method_get_param);
    BIND(method_is_instance,     t_method_is_instance);
    BIND(field_get_name,         t_field_get_name);
    BIND(field_get_type,         t_field_get_type);
    BIND(field_get_offset,       t_field_get_offset);
    BIND(field_get_flags,        t_field_get_flags);
    BIND(field_static_get_value, t_field_static_get_value);
    BIND(type_get_name,          t_type_get_name);
    BIND(type_is_byref,          t_type_is_byref);
    BIND(type_get_object,        t_type_get_object);
    BIND(class_from_type,        t_class_from_type);
    BIND(get_all_assemblies,        t_get_all_assemblies);
    BIND(get_assembly_type_handle,  t_get_assembly_type_handle);
    BIND(get_type_info_from_handle, t_get_type_info_from_handle);
    return true;
}
#undef BIND

Il2CppClass* find_class(const char* ns, const char* name) {
    if (!get_all_assemblies || !get_assembly_type_handle) return nullptr;
    size_t n = 0;
    auto asm_ = get_all_assemblies(&n);
    if (!asm_) return nullptr;
    char full[512];
    if (ns && *ns) snprintf(full, sizeof(full), "%s.%s", ns, name);
    else           snprintf(full, sizeof(full), "%s", name);
    for (size_t i = 0; i < n; ++i) {
        auto k = get_assembly_type_handle(asm_[i], full);
        if (k) return k;
    }
    return nullptr;
}

Il2CppMethod* find_method(Il2CppClass* klass, const char* name) {
    if (!klass) return nullptr;
    void* it = nullptr;
    while (auto m = class_get_methods(klass, &it)) {
        const char* n = method_get_name(m);
        if (n && !strcmp(n, name)) return (Il2CppMethod*)m;
    }
    return nullptr;
}

Il2CppField* find_field(Il2CppClass* klass, const char* name) {
    if (!klass) return nullptr;
    void* it = nullptr;
    while (auto f = class_get_fields(klass, &it)) {
        const char* n = field_get_name(f);
        if (n && !strcmp(n, name)) return (Il2CppField*)f;
    }
    return nullptr;
}

void* method_pointer(Il2CppMethod* m) {
    return m ? *reinterpret_cast<void**>(m) : nullptr;
}
uint32_t field_offset(Il2CppClass* k, const char* n) {
    auto f = find_field(k, n);
    return f ? field_get_offset(f) : 0;
}

} // namespace il2cpp
