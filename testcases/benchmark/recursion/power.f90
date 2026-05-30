recursive function power(base, exp) result(res)
    integer, intent(in) :: base, exp
    integer(8) :: res, half
    if (exp == 0) then
        res = 1
        return
    end if
    if (mod(exp, 2) == 0) then
        half = power(base, exp / 2)
        res = half * half
    else
        res = base * power(base, exp - 1)
    end if
end function power

program main
end program main
