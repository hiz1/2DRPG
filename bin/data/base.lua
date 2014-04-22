threadTable = {}

counter = 0
function addThread(thr)
  table.insert(threadTable, thr)
  return table.maxn(threadTable)
end

function removeThread(threadIdx)
  threadTable[threadIdx] = nil
end

function wait(n)
  for idx = 1, n do
    coroutine.yield()
  end
end

function mv(x, y, frame)
  for idx = 1, frame do
    cmove(x/frame,y/frame)
    coroutine.yield()
  end
end