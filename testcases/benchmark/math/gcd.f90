function gcd(a, b) result(res)
    integer, intent(in) :: a, b
    integer :: res, x, y, temp
    x = a
    y = b
    do while (y /= 0)
        temp = y
        y = mod(x, y)
        x = temp
    end do
    res = x
end function gcd

program main
end program main
