function fibonacci(n) result(res)
    integer, intent(in) :: n
    integer :: res, a, b, temp, i
    if (n <= 1) then
        res = n
        return
    end if
    a = 0
    b = 1
    do i = 2, n
        temp = a + b
        a = b
        b = temp
    end do
    res = b
end function fibonacci

program main
end program main
