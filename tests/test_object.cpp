/**************************************************************************/
/*                            BEYOTA ENGINE                               */
/**************************************************************************/
/*  test_object.cpp                                                       */
/**************************************************************************/

#include "core/object/class_db.h"
#include "core/object/method_bind.h"
#include "core/object/object.h"
#include "core/object/object_db.h"
#include "core/object/object_id.h"
#include "core/object/property_info.h"
#include "core/object/ref_counted.h"
#include "core/variant/variant.h"

#include <atomic>
#include <cassert>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using namespace Beyota;

// Test hierarchy classes
class TestNode : public Object {
    int value_{0};
    int notification_received_{-1};

public:
    static inline std::atomic<int> live_instances{0};

    TestNode() noexcept {
        live_instances.fetch_add(1, std::memory_order_relaxed);
    }

    ~TestNode() override {
        live_instances.fetch_sub(1, std::memory_order_relaxed);
    }

    static StringName get_class_name_static() {
        return StringName("TestNode");
    }

    [[nodiscard]] StringName get_class_name() const override {
        return get_class_name_static();
    }

    void on_notification(int p_what) override {
        notification_received_ = p_what;
    }

    [[nodiscard]] int get_notification_received() const noexcept {
        return notification_received_;
    }

    void set_value(int p_val) noexcept {
        value_ = p_val;
    }

    [[nodiscard]] int get_value() const noexcept {
        return value_;
    }

    int add(int a, int b) const noexcept {
        return a + b;
    }

    int multiply_with_default(int a, int b) const noexcept {
        return a * b;
    }

    static int static_compute(int x) noexcept {
        return x * 10;
    }
};

class TestDerivedNode : public TestNode {
    String tag_{};

public:
    static inline std::atomic<int> live_derived{0};

    TestDerivedNode() noexcept {
        live_derived.fetch_add(1, std::memory_order_relaxed);
    }

    ~TestDerivedNode() override {
        live_derived.fetch_sub(1, std::memory_order_relaxed);
    }

    static StringName get_class_name_static() {
        return StringName("TestDerivedNode");
    }

    [[nodiscard]] StringName get_class_name() const override {
        return get_class_name_static();
    }

    void set_tag(const String &p_tag) {
        tag_ = p_tag;
    }

    [[nodiscard]] String get_tag() const {
        return tag_;
    }
};

class TestResource : public RefCounted {
    int data_{0};

public:
    static inline std::atomic<int> live_resources{0};

    TestResource() noexcept {
        live_resources.fetch_add(1, std::memory_order_relaxed);
    }

    explicit TestResource(int p_data) noexcept : data_(p_data) {
        live_resources.fetch_add(1, std::memory_order_relaxed);
    }

    ~TestResource() override {
        live_resources.fetch_sub(1, std::memory_order_relaxed);
    }

    static StringName get_class_name_static() {
        return StringName("TestResource");
    }

    [[nodiscard]] StringName get_class_name() const override {
        return get_class_name_static();
    }

    void set_data(int p_data) noexcept {
        data_ = p_data;
    }

    [[nodiscard]] int get_data() const noexcept {
        return data_;
    }
};

class TestSubResource : public TestResource {
    String extra_{};

public:
    static inline std::atomic<int> live_sub_resources{0};

    TestSubResource() noexcept {
        live_sub_resources.fetch_add(1, std::memory_order_relaxed);
    }

    ~TestSubResource() override {
        live_sub_resources.fetch_sub(1, std::memory_order_relaxed);
    }

    static StringName get_class_name_static() {
        return StringName("TestSubResource");
    }

    [[nodiscard]] StringName get_class_name() const override {
        return get_class_name_static();
    }

    void set_extra(const String &p_extra) {
        extra_ = p_extra;
    }

    [[nodiscard]] String get_extra() const {
        return extra_;
    }
};

static void test_suite_object_id() {
    ObjectID null_id;
    assert(null_id.is_null());
    assert(!null_id.is_valid());
    assert(!null_id.is_ref_counted());
    assert((u64)null_id == 0);
    assert((i64)null_id == 0);

    ObjectID valid_id(42ULL);
    assert(!valid_id.is_null());
    assert(valid_id.is_valid());
    assert(!valid_id.is_ref_counted());
    assert((u64)valid_id == 42ULL);
    assert((i64)valid_id == 42LL);

    ObjectID ref_id(42ULL | ObjectID::REF_COUNTED_FLAG);
    assert(ref_id.is_ref_counted());
    assert(ref_id.is_valid());

    assert(null_id != valid_id);
    assert(valid_id < ref_id);
    assert(valid_id.hash() != 0);

    std::cout << "[PASS] ObjectID creation, refcount bit, hashes & operators\n";
}

static void test_suite_object_db() {
    ObjectDB::setup();
    u32 initial_count = ObjectDB::get_object_count();

    TestNode *node1 = new TestNode();
    ObjectID id1 = node1->get_instance_id();
    assert(id1.is_valid());
    assert(!id1.is_ref_counted());
    assert(ObjectDB::get_object_count() == initial_count + 1);

    Object *lookup1 = ObjectDB::get_instance(id1);
    assert(lookup1 == node1);
    TestNode *cast1 = ObjectDB::get_instance<TestNode>(id1);
    assert(cast1 == node1);

    // Delete node1, slot validator should increment and invalidate id1
    delete node1;
    assert(ObjectDB::get_instance(id1) == nullptr);
    assert(ObjectDB::get_object_count() == initial_count);

    // Create node2 which will reuse slot with new validator
    TestNode *node2 = new TestNode();
    ObjectID id2 = node2->get_instance_id();
    assert(id2.is_valid());
    assert(id2 != id1); // different validator!
    assert(ObjectDB::get_instance(id1) == nullptr); // stale id still null
    assert(ObjectDB::get_instance(id2) == node2);
    delete node2;
    assert(ObjectDB::get_instance(id2) == nullptr);

    // Multithreaded ObjectDB instance creation and deletion stress
    constexpr int NUM_THREADS = 8;
    constexpr int OBJECTS_PER_THREAD = 200;
    std::vector<std::thread> threads;
    std::atomic<bool> start{false};

    for (int t = 0; t < NUM_THREADS; ++t) {
        threads.emplace_back([&start]() {
            while (!start.load(std::memory_order_relaxed)) {}
            std::vector<TestNode *> nodes;
            nodes.reserve(OBJECTS_PER_THREAD);
            for (int i = 0; i < OBJECTS_PER_THREAD; ++i) {
                TestNode *n = new TestNode();
                assert(ObjectDB::get_instance(n->get_instance_id()) == n);
                nodes.push_back(n);
            }
            for (TestNode *n : nodes) {
                ObjectID nid = n->get_instance_id();
                delete n;
                assert(ObjectDB::get_instance(nid) == nullptr);
            }
        });
    }

    start.store(true, std::memory_order_relaxed);
    for (auto &th : threads) {
        th.join();
    }

    assert(ObjectDB::get_object_count() == initial_count);
    std::cout << "[PASS] ObjectDB generational slot map & concurrency safety\n";
}

static void test_suite_object_lifecycle() {
    ClassDB::setup();
    ClassDB::register_class<TestNode, Object>();
    ClassDB::register_class<TestDerivedNode, TestNode>();

    assert(TestNode::live_instances == 0);
    assert(TestDerivedNode::live_derived == 0);

    {
        TestDerivedNode derived;
        assert(TestNode::live_instances == 1);
        assert(TestDerivedNode::live_derived == 1);

        // Class name & hierarchy
        assert(derived.get_class_name() == StringName("TestDerivedNode"));
        assert(derived.is_class(StringName("TestDerivedNode")));
        assert(derived.is_class(StringName("TestNode")));
        assert(derived.is_class(StringName("Object")));
        assert(!derived.is_class(StringName("TestResource")));

        // cast_to
        Object *base_ptr = &derived;
        TestNode *node_ptr = Object::cast_to<TestNode>(base_ptr);
        assert(node_ptr != nullptr);
        TestDerivedNode *derived_ptr = Object::cast_to<TestDerivedNode>(base_ptr);
        assert(derived_ptr != nullptr);
        TestResource *res_ptr = Object::cast_to<TestResource>(base_ptr);
        assert(res_ptr == nullptr);

        // Notifications
        derived.notification(Object::NOTIFICATION_POSTINITIALIZE);
        assert(derived.get_notification_received() == Object::NOTIFICATION_POSTINITIALIZE);

        derived.notification(42);
        assert(derived.get_notification_received() == 42);

        // Metadata
        assert(!derived.has_meta(StringName("author")));
        derived.set_meta(StringName("author"), Variant(String("BeyotaDev")));
        assert(derived.has_meta(StringName("author")));
        assert(derived.get_meta(StringName("author")) == Variant(String("BeyotaDev")));
        assert(derived.get_meta(StringName("nonexistent"), Variant(100)) == Variant(100));

        auto meta_list = derived.get_meta_list();
        assert(meta_list.size() == 1);
        assert(meta_list[0] == StringName("author"));

        assert(derived.remove_meta(StringName("author")));
        assert(!derived.has_meta(StringName("author")));
        assert(!derived.remove_meta(StringName("author")));

        // to_string
        String str = derived.to_string();
        assert(str.length() > 0);
    }

    assert(TestNode::live_instances == 0);
    assert(TestDerivedNode::live_derived == 0);
    std::cout << "[PASS] Object lifecycle, notifications, metadata & cast_to\n";
}

static void test_suite_ref_counted_and_ref() {
    assert(TestResource::live_resources == 0);
    assert(TestSubResource::live_sub_resources == 0);

    // Basic Ref lifecycle
    {
        Ref<TestResource> r1(new TestResource(100));
        assert(r1.is_valid());
        assert(!r1.is_null());
        assert((bool)r1);
        assert(r1->get_data() == 100);
        assert((*r1).get_data() == 100);
        assert(r1.ptr()->get_reference_count() == 1);
        assert(TestResource::live_resources == 1);

        {
            Ref<TestResource> r2 = r1; // Copy constructor
            assert(r2.is_valid());
            assert(r1.ptr()->get_reference_count() == 2);
            assert(r1 == r2);
            assert(r1 == r1.ptr());

            Ref<TestResource> r3;
            r3 = r2; // Copy assignment
            assert(r1.ptr()->get_reference_count() == 3);

            // Self-assignment
            Ref<TestResource> &r2_alias = r2;
            r2 = r2_alias;
            assert(r1.ptr()->get_reference_count() == 3);
        } // r2, r3 go out of scope

        assert(r1.ptr()->get_reference_count() == 1);
        assert(TestResource::live_resources == 1);
    } // r1 goes out of scope, deletes TestResource

    assert(TestResource::live_resources == 0);

    // Move semantics
    {
        Ref<TestResource> r1(new TestResource(50));
        assert(TestResource::live_resources == 1);

        Ref<TestResource> r2 = std::move(r1); // Move constructor
        assert(!r1.is_valid());
        assert(r1.is_null());
        assert(r2.is_valid());
        assert(r2->get_data() == 50);
        assert(r2.ptr()->get_reference_count() == 1);

        Ref<TestResource> r3;
        r3 = std::move(r2); // Move assignment
        assert(!r2.is_valid());
        assert(r3.is_valid());
        assert(r3->get_data() == 50);
        assert(r3.ptr()->get_reference_count() == 1);
    }
    assert(TestResource::live_resources == 0);

    // Derived Ref conversions (Ref<TestSubResource> to Ref<TestResource>)
    {
        Ref<TestSubResource> sub(new TestSubResource());
        sub->set_data(777);
        sub->set_extra(String("Special"));
        assert(TestResource::live_resources == 1);
        assert(TestSubResource::live_sub_resources == 1);
        assert(sub.ptr()->get_reference_count() == 1);

        // Converting copy constructor
        Ref<TestResource> base = sub;
        assert(base.is_valid());
        assert(base->get_data() == 777);
        assert(sub.ptr()->get_reference_count() == 2);

        // Downcast via cast_to
        Ref<TestSubResource> casted = base.cast_to<TestSubResource>();
        assert(casted.is_valid());
        assert(casted->get_extra() == String("Special"));
        assert(sub.ptr()->get_reference_count() == 3);

        // Incompatible downcast returns null
        Ref<TestResource> plain(new TestResource(1));
        Ref<TestSubResource> invalid_cast = plain.cast_to<TestSubResource>();
        assert(invalid_cast.is_null());
    }
    assert(TestResource::live_resources == 0);
    assert(TestSubResource::live_sub_resources == 0);

    // Multithreaded RefCounted atomic operations
    {
        Ref<TestResource> shared_res(new TestResource(42));
        constexpr int NUM_WORKERS = 8;
        constexpr int REFS_PER_WORKER = 500;
        std::vector<std::thread> workers;
        std::atomic<bool> go{false};

        for (int w = 0; w < NUM_WORKERS; ++w) {
            workers.emplace_back([shared_res, &go]() {
                while (!go.load(std::memory_order_relaxed)) {}
                for (int i = 0; i < REFS_PER_WORKER; ++i) {
                    Ref<TestResource> local = shared_res;
                    assert(local.is_valid());
                    assert(local->get_data() == 42);
                }
            });
        }

        go.store(true, std::memory_order_relaxed);
        for (auto &w : workers) {
            w.join();
        }

        assert(shared_res.ptr()->get_reference_count() == 1);
    }
    assert(TestResource::live_resources == 0);

    std::cout << "[PASS] RefCounted atomic lifecycle, Ref<T> RAII & polymorphism\n";
}

static void test_suite_class_db() {
    ClassDB::setup();

    // Register hierarchy
    ClassDB::register_class<TestNode, Object>();
    ClassDB::register_class<TestDerivedNode, TestNode>();
    ClassDB::register_class<TestResource, RefCounted>();
    ClassDB::register_class<TestSubResource, TestResource>();

    assert(ClassDB::is_class_registered(StringName("TestNode")));
    assert(ClassDB::is_class_registered(StringName("TestDerivedNode")));
    assert(ClassDB::is_parent_class(StringName("TestDerivedNode"), StringName("TestNode")));
    assert(ClassDB::is_parent_class(StringName("TestDerivedNode"), StringName("Object")));
    assert(!ClassDB::is_parent_class(StringName("TestNode"), StringName("TestDerivedNode")));

    // Dynamic instantiation
    assert(ClassDB::can_instantiate(StringName("TestDerivedNode")));
    Object *inst = ClassDB::instantiate(StringName("TestDerivedNode"));
    assert(inst != nullptr);
    assert(inst->get_class_name() == StringName("TestDerivedNode"));
    delete inst;

    // Method binding
    ClassDB::bind_method(StringName("set_value"), &TestNode::set_value);
    ClassDB::bind_method(StringName("get_value"), &TestNode::get_value);
    ClassDB::bind_method(StringName("add"), &TestNode::add);
    ClassDB::bind_method(StringName("multiply_with_default"), &TestNode::multiply_with_default,
                         {Variant(2)}); // Default argument b = 2
    ClassDB::bind_static_method(StringName("TestNode"), StringName("static_compute"),
                                &TestNode::static_compute);

    assert(ClassDB::has_method(StringName("TestNode"), StringName("set_value")));
    assert(ClassDB::has_method(StringName("TestDerivedNode"), StringName("set_value"))); // Inherited

    // Dynamic method invocation
    TestDerivedNode target;
    Callable::CallError err;

    // Call set_value(123)
    Variant arg1(123);
    const Variant *args1[] = {&arg1};
    ClassDB::call(&target, StringName("set_value"), args1, 1, err);
    assert(err.error == Callable::CallError::CALL_OK);
    assert(target.get_value() == 123);

    // Call get_value() -> 123
    Variant ret = ClassDB::call(&target, StringName("get_value"), nullptr, 0, err);
    assert(err.error == Callable::CallError::CALL_OK);
    assert(ret == Variant(123));

    // Call add(10, 25) -> 35
    Variant a(10), b(25);
    const Variant *args_add[] = {&a, &b};
    Variant sum = ClassDB::call(&target, StringName("add"), args_add, 2, err);
    assert(err.error == Callable::CallError::CALL_OK);
    assert(sum == Variant(35));

    // Call multiply_with_default(7) -> uses default b = 2 => 14
    Variant m1(7);
    const Variant *args_m1[] = {&m1};
    Variant prod1 = ClassDB::call(&target, StringName("multiply_with_default"), args_m1, 1, err);
    assert(err.error == Callable::CallError::CALL_OK);
    assert(prod1 == Variant(14));

    // Call multiply_with_default(7, 5) => 35
    Variant m2(5);
    const Variant *args_m2[] = {&m1, &m2};
    Variant prod2 = ClassDB::call(&target, StringName("multiply_with_default"), args_m2, 2, err);
    assert(err.error == Callable::CallError::CALL_OK);
    assert(prod2 == Variant(35));

    // Error: too few arguments
    ClassDB::call(&target, StringName("add"), args1, 1, err);
    assert(err.error == Callable::CallError::CALL_ERROR_TOO_FEW_ARGUMENTS);

    // Error: too many arguments
    ClassDB::call(&target, StringName("get_value"), args1, 1, err);
    assert(err.error == Callable::CallError::CALL_ERROR_TOO_MANY_ARGUMENTS);

    // Error: invalid method
    ClassDB::call(&target, StringName("non_existent_method"), nullptr, 0, err);
    assert(err.error == Callable::CallError::CALL_ERROR_INVALID_METHOD);

    // Call via Object::call directly
    Variant v_call = target.call(StringName("add"), 100, 50);
    assert(v_call == Variant(150));

    // Property registration & access
    ClassDB::add_property(StringName("TestNode"),
                          PropertyInfo(VariantType::INT, "value"),
                          StringName("set_value"),
                          StringName("get_value"));

    assert(ClassDB::has_property(StringName("TestNode"), StringName("value")));
    assert(ClassDB::has_property(StringName("TestDerivedNode"), StringName("value")));

    bool set_ok = target.set(StringName("value"), Variant(999));
    assert(set_ok);
    assert(target.get_value() == 999);

    bool get_valid = false;
    Variant got = target.get(StringName("value"), &get_valid);
    assert(get_valid);
    assert(got == Variant(999));

    std::vector<PropertyInfo> prop_list;
    target.get_property_list(&prop_list);
    assert(!prop_list.empty());

    ClassDB::cleanup();
    std::cout << "[PASS] ClassDB reflection, method/property binding & dynamic invocation\n";
}

static void test_suite_variant_object_integration() {
    ClassDB::setup();
    ClassDB::register_class<TestNode, Object>();
    ClassDB::register_class<TestResource, RefCounted>();
    ClassDB::bind_method(StringName("set_value"), &TestNode::set_value);
    ClassDB::bind_method(StringName("get_value"), &TestNode::get_value);

    // 1. Storing raw Object* in Variant and dangling pointer safety
    {
        TestNode *raw_node = new TestNode();
        raw_node->set_value(555);

        Variant var_obj(raw_node);
        assert(var_obj.get_type() == Variant::OBJECT);
        assert((Object *)var_obj == raw_node);

        delete raw_node;
        // After deletion, ObjectDB::get_instance returns nullptr
        assert((Object *)var_obj == nullptr);
    }

    // 2. Storing RefCounted in Variant: automatic refcount management
    assert(TestResource::live_resources == 0);
    {
        Ref<TestResource> res(new TestResource(888));
        assert(TestResource::live_resources == 1);
        assert(res.ptr()->get_reference_count() == 1);

        {
            Variant v_res = res; // Convert Ref<T> to Variant
            assert(v_res.get_type() == Variant::OBJECT);
            assert(res.ptr()->get_reference_count() == 2);

            {
                Variant v_copy = v_res; // Variant copy
                assert(res.ptr()->get_reference_count() == 3);

                // Extract Ref from Variant
                Ref<TestResource> extracted = v_copy;
                assert(extracted.is_valid());
                assert(extracted->get_data() == 888);
                assert(res.ptr()->get_reference_count() == 4);
            } // v_copy and extracted go out of scope
            assert(res.ptr()->get_reference_count() == 2);
        } // v_res goes out of scope
        assert(res.ptr()->get_reference_count() == 1);
    } // res goes out of scope
    assert(TestResource::live_resources == 0); // Completely reclaimed!

    // 3. Variant created directly from new RefCounted adopts and frees it
    {
        Variant v_anon(new TestResource(999));
        assert(TestResource::live_resources == 1);
        Ref<TestResource> r = v_anon;
        assert(r.is_valid());
        assert(r->get_data() == 999);
    }
    assert(TestResource::live_resources == 0);

    // 4. Callable dispatching to Object through ObjectDB
    {
        TestNode target_node;
        target_node.set_value(10);

        Callable callable(&target_node, StringName("set_value"));
        assert(callable.is_valid());
        assert(callable.get_object_id() == target_node.get_instance_id());
        assert(callable.get_method() == StringName("set_value"));

        Variant arg(456);
        const Variant *cargs[] = {&arg};
        Variant ret;
        Callable::CallError err;
        callable.callp(cargs, 1, ret, err);
        assert(err.error == Callable::CallError::CALL_OK);
        assert(target_node.get_value() == 456);

        // Callable callv with Array
        Array arr;
        arr.push_back(Variant(789));
        callable.callv(arr);
        assert(target_node.get_value() == 789);
    }

    // 5. Signals connecting and emitting to Callables
    {
        TestNode emitter;
        TestNode receiver;
        receiver.set_value(0);

        Callable receiver_slot(&receiver, StringName("set_value"));
        emitter.connect(StringName("on_change"), receiver_slot);
        assert(emitter.is_connected(StringName("on_change"), receiver_slot));

        // Emit signal
        emitter.emit_signal(StringName("on_change"), 321);
        assert(receiver.get_value() == 321);

        // Disconnect
        emitter.disconnect(StringName("on_change"), receiver_slot);
        assert(!emitter.is_connected(StringName("on_change"), receiver_slot));

        emitter.emit_signal(StringName("on_change"), 654);
        assert(receiver.get_value() == 321); // Not modified
    }

    ClassDB::cleanup();
    std::cout << "[PASS] Variant integration, automatic RefCounted tracking & Signals\n";
}

int main() {
    std::cout << "Running exhaustive Beyota core/object subsystem test suite...\n";
    std::cout << "=========================================================\n";

    test_suite_object_id();
    test_suite_object_db();
    test_suite_object_lifecycle();
    test_suite_ref_counted_and_ref();
    test_suite_class_db();
    test_suite_variant_object_integration();

    std::cout << "=========================================================\n";
    std::cout << "ALL 6 OBJECT SUBSYSTEM TEST SUITES PASSED SUCCESSFULLY!\n";
    std::cout << "=========================================================\n";
    return 0;
}
