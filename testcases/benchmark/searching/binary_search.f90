function binary_search(arr, n, target) result(idx)
    integer, intent(in) :: arr(:), n, target
    integer :: idx, low, high, mid
    low = 1
    high = n
    idx = -1
    do while (low <= high)
        mid = low + (high - low) / 2
        if (arr(mid) == target) then
            idx = mid
            return
        else if (arr(mid) < target) then
            low = mid + 1
        else
            high = mid - 1
        end if
    end do
end function binary_search

program main
end program main
