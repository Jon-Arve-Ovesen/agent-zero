"""Unit tests for python/helpers/rate_limiter.py."""
from __future__ import annotations

import asyncio
import sys
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parents[1]
if str(PROJECT_ROOT) not in sys.path:
    sys.path.insert(0, str(PROJECT_ROOT))

import pytest

from python.helpers.rate_limiter import RateLimiter


# ---------------------------------------------------------------------------
# Synchronous / construction tests
# ---------------------------------------------------------------------------


def test_rate_limiter_initialises_with_limits():
    rl = RateLimiter(seconds=60, requests=10, tokens=1000)
    assert rl.timeframe == 60
    assert rl.limits["requests"] == 10
    assert rl.limits["tokens"] == 1000


def test_rate_limiter_add_creates_key_on_first_call():
    rl = RateLimiter(seconds=60, requests=5)
    rl.add(requests=1)
    assert "requests" in rl.values
    assert len(rl.values["requests"]) == 1


def test_rate_limiter_add_accumulates_entries():
    rl = RateLimiter(seconds=60, requests=5)
    rl.add(requests=1)
    rl.add(requests=2)
    rl.add(requests=3)
    assert len(rl.values["requests"]) == 3


def test_rate_limiter_add_accepts_unknown_key_via_setdefault():
    """Keys not in limits can still be tracked via add()."""
    rl = RateLimiter(seconds=60)
    rl.add(extra=7)
    assert "extra" in rl.values
    assert rl.values["extra"][0][1] == 7


# ---------------------------------------------------------------------------
# Async tests
# ---------------------------------------------------------------------------


@pytest.mark.asyncio
async def test_get_total_returns_zero_for_unknown_key():
    rl = RateLimiter(seconds=60, requests=10)
    total = await rl.get_total("nonexistent")
    assert total == 0


@pytest.mark.asyncio
async def test_get_total_sums_values():
    rl = RateLimiter(seconds=60, requests=100)
    rl.add(requests=3)
    rl.add(requests=5)
    total = await rl.get_total("requests")
    assert total == 8


@pytest.mark.asyncio
async def test_cleanup_removes_expired_entries():
    import asyncio

    rl = RateLimiter(seconds=1, requests=100)
    rl.add(requests=5)
    assert len(rl.values["requests"]) == 1

    # Wait for the timeframe to expire then clean up.
    await asyncio.sleep(1.05)
    await rl.cleanup()
    assert len(rl.values.get("requests", [])) == 0


@pytest.mark.asyncio
async def test_wait_returns_immediately_when_under_limit():
    rl = RateLimiter(seconds=60, requests=10)
    rl.add(requests=5)
    # Should complete without sleeping because 5 < 10
    await asyncio.wait_for(rl.wait(), timeout=2.0)


@pytest.mark.asyncio
async def test_wait_invokes_callback_when_limit_exceeded():
    calls: list[str] = []

    async def cb(msg: str, key: str, total: int, limit: int) -> bool:
        calls.append(key)
        return True  # returning True signals "proceed" (should_wait = not True = False)

    rl = RateLimiter(seconds=60, requests=5)
    rl.add(requests=10)  # exceeds limit of 5
    await rl.wait(callback=cb)
    assert calls == ["requests"]
