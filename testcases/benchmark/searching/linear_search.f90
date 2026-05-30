function linear_search(arr, n, target) result(idx)
    integer, intent(in) :: arr(:), n, target
    integer :: idx, i
    idx = -1
    do i = 1, n
        if (arr(i) == target) then
            idx = i
            return
        end if
    end do
end function linear_search

program main
end program main
