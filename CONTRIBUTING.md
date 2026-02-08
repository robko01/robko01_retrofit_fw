# Contributing

Thanks for contributing to `robko01_retrofit_fw`.

## Branching strategy

- `main`: production-ready code, receives merges from `dev`
- `dev`: integration branch, created from `main`
- Feature branches: created from `dev` for each change

Branch naming:

- Features: `feature/<short-description>`
- Fixes: `fix/<short-description>`

## Contribution workflow

1. Checkout `dev`
2. Create a feature/fix branch from `dev`
3. Implement changes
4. Update `CHANGELOG.md` and version (when applicable)
5. Build/test your changes
6. Merge feature branch into `dev` with `--no-ff`
7. Merge `dev` into `main` with `--no-ff`
8. Push `main` and `dev`, then delete local feature branch

Recommended merge commands:

```bash
git checkout dev
git merge feature/<short-description> --no-ff -m "Merge feature/<short-description> into dev"

git checkout main
git merge dev --no-ff -m "Merge dev into main"
```

## Commit message format

Use imperative, concise commit titles (max ~50 chars), then bullets:

```text
Short summary (imperative mood, max 50 chars)

- Bullet point describing change 1
- Bullet point describing change 2
- Bullet point describing change 3
```

Examples:

- `Add WDT integration for motor safety`
- `Fix serial communication timeout handling`
- `Enable limits and estop for TCM profile`

## Testing expectations

- Software-only changes: run a build and verify behavior
- Hardware-dependent changes: validate on target hardware before final merge

Useful commands:

```bash
pio run
pio run -e <env>
pio run -t upload
pio test
```

## Versioning and changelog

Version source: `include/Version.h`

- `FW_VERSION_MAJOR`: breaking changes
- `FW_VERSION_MINOR`: new features/significant improvements
- `FW_BUILD_NUMBER`: increment on each merge to `main`

When merging to `main`:

1. Increment `FW_BUILD_NUMBER`
2. Add/complete the release entry in `CHANGELOG.md`

Keep changelog entries in [Keep a Changelog](https://keepachangelog.com/) style sections:

- `Added`
- `Changed`
- `Deprecated`
- `Removed`
- `Fixed`
- `Security`

## Security and secrets

- Never commit secrets
- `.env` is git-ignored and should stay local
