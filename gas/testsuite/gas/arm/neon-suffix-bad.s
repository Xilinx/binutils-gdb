.syntax unified
.arm
add.f32 r0, r0, r0
faddd.f32 d0, d0, d0
faddd.f64 d0, d0, d0

.thumb
add.f32 r0, r0, r0
faddd.f32 d0, d0, d0
faddd.f64 d0, d0, d0


