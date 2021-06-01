#pragma once

namespace egl
{
    class Ref_counted_base
    {
    private:
        mutable long mCount;
        Ref_counted_base(Ref_counted_base const &);
        Ref_counted_base &operator=(Ref_counted_base const &);

    protected:
        Ref_counted_base() : mCount(0) {}
        virtual ~Ref_counted_base() {}

    public:
        void AddRef() const { ++mCount; }
        void Release() const
        {
            if (--mCount == 0)
                delete this;
        }

        long RefCount() const { return mCount; }
    };

    inline void intrusive_ptr_add_ref(Ref_counted_base const *p) { p->AddRef(); }
    inline void intrusive_ptr_release(Ref_counted_base const *p) { p->Release(); }
}