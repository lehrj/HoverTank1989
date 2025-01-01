//--------------------------------------------------------------------------------------
// File: AnimatedTexture.h
//
// A C++ version of the C# example on how to animate a 2D sprite using SpriteBatch:
// http://msdn.microsoft.com/en-us/library/bb203866.aspx
//
// This class manages a sprite-based 2D animation for DirectX using the SpriteBatch API.
// It divides a single texture into frames, updates the current frame based on elapsed time,
// and draws the appropriate region of the texture.
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//--------------------------------------------------------------------------------------

#pragma once

#include <stdexcept>
#include <SpriteBatch.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXColors.h>

/**
 * @class AnimatedTexture
 * @brief Manages a sprite-based texture animation (frame by frame) using DirectX SpriteBatch.
 */
class AnimatedTexture
{
public:
    /**
     * @brief Default constructor (no texture loaded).
     *        Initializes all members to safe defaults (e.g., paused = false, frame = 0, etc.).
     */
    AnimatedTexture() noexcept
        : mPaused(false)
        , mFrame(0)
        , mFrameCount(0)
        , mTextureWidth(0)
        , mTextureHeight(0)
        , mTimePerFrame(0.0f)
        , mTotalElapsed(0.0f)
        , mDepth(0.0f)
        , mRotation(0.0f)
        , mOrigin{}
        , mScale(1.0f, 1.0f)
    {
    }

    /**
     * @brief Overloaded constructor allowing custom origin, rotation, scale, and depth.
     * @param origin   The pivot/origin for rotation, typically center of the sprite.
     * @param rotation The rotation in radians.
     * @param scale    Uniform scale factor (applied to both X and Y).
     * @param depth    Depth layer used by SpriteBatch (0.0 is front, 1.0 is back).
     */
    AnimatedTexture(const DirectX::XMFLOAT2& origin,
                    float rotation,
                    float scale,
                    float depth) noexcept
        : mPaused(false)
        , mFrame(0)
        , mFrameCount(0)
        , mTextureWidth(0)
        , mTextureHeight(0)
        , mTimePerFrame(0.0f)
        , mTotalElapsed(0.0f)
        , mDepth(depth)
        , mRotation(rotation)
        , mOrigin(origin)
        , mScale(scale, scale)
    {
    }

    // Use default move and copy semantics
    AnimatedTexture(AnimatedTexture&&) = default;
    AnimatedTexture& operator=(AnimatedTexture&&) = default;
    AnimatedTexture(const AnimatedTexture&) = default;
    AnimatedTexture& operator=(const AnimatedTexture&) = default;

    /**
     * @brief Loads a texture (ID3D11ShaderResourceView) and sets up frame count / FPS.
     * @param texture          The shader resource view for the texture.
     * @param frameCount       Number of frames (must be > 0).
     * @param framesPerSecond  Animation speed in frames per second (must be > 0).
     * @throws std::invalid_argument if frameCount < 0 or framesPerSecond <= 0.
     * @throws std::runtime_error if texture is not a Texture2D resource.
     */
    void Load(ID3D11ShaderResourceView* texture, int frameCount, int framesPerSecond)
    {
        if (frameCount < 0 || framesPerSecond <= 0)
        {
            throw std::invalid_argument("AnimatedTexture::Load - Invalid frameCount/FPS values.");
        }

        mPaused = false;
        mFrame = 0;
        mFrameCount = frameCount;
        mTimePerFrame = 1.0f / static_cast<float>(framesPerSecond);
        mTotalElapsed = 0.0f;
        mTexture = texture;

        if (texture)
        {
            // Obtain the underlying resource (expected to be a 2D texture)
            Microsoft::WRL::ComPtr<ID3D11Resource> resource;
            texture->GetResource(resource.GetAddressOf());

            D3D11_RESOURCE_DIMENSION dim;
            resource->GetType(&dim);

            if (dim != D3D11_RESOURCE_DIMENSION_TEXTURE2D)
            {
                throw std::runtime_error("AnimatedTexture::Load - Resource is not a Texture2D.");
            }

            // Extract the texture dimensions
            Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D;
            resource.As(&tex2D);

            D3D11_TEXTURE2D_DESC desc{};
            tex2D->GetDesc(&desc);

            mTextureWidth = static_cast<int>(desc.Width);
            mTextureHeight = static_cast<int>(desc.Height);
        }
    }

    /**
     * @brief Updates the current animation frame based on the elapsed time.
     * @param elapsed Time in seconds since last frame (delta time).
     */
    void Update(float elapsed)
    {
        if (mPaused)
        {
            return;
        }

        mTotalElapsed += elapsed;

        // Advance to next frame if enough time has passed
        if (mTotalElapsed > mTimePerFrame)
        {
            ++mFrame;
            mFrame %= mFrameCount; // wrap around
            mTotalElapsed -= mTimePerFrame;
        }
    }

    /**
     * @brief Draws the current frame of the animation at a specified screen position.
     * @param batch     Pointer to a valid SpriteBatch instance.
     * @param screenPos The position on the screen (in pixels).
     */
    void Draw(DirectX::SpriteBatch* batch, const DirectX::XMFLOAT2& screenPos) const
    {
        Draw(batch, mFrame, screenPos);
    }

    /**
     * @brief Draws a specific frame of the animation at a specified screen position.
     * @param batch     Pointer to a valid SpriteBatch instance.
     * @param frame     The frame index to draw.
     * @param screenPos The position on the screen (in pixels).
     */
    void Draw(DirectX::SpriteBatch* batch, int frame, const DirectX::XMFLOAT2& screenPos) const
    {
        if (!mTexture)
        {
            // Optionally, you could skip drawing or throw an error
            // throw std::runtime_error("AnimatedTexture::Draw - Texture is not set.");
            return;
        }

        // Safeguard against invalid frame indices
        if (frame < 0 || frame >= mFrameCount)
        {
            // Optionally clamp or wrap the frame index
            frame = (frame >= mFrameCount) ? (mFrameCount - 1) : 0;
        }

        // Calculate each frame's width
        int frameWidth = (mFrameCount > 0) ? (mTextureWidth / mFrameCount) : mTextureWidth;

        RECT sourceRect{};
        sourceRect.left = frameWidth * frame;
        sourceRect.top = 0;
        sourceRect.right = sourceRect.left + frameWidth;
        sourceRect.bottom = mTextureHeight;

        batch->Draw(
            mTexture.Get(),
            screenPos,
            &sourceRect,
            DirectX::Colors::White,
            mRotation,
            mOrigin,
            mScale,
            DirectX::SpriteEffects_None,
            mDepth
        );
    }

    /**
     * @brief Resets the animation to the first frame and time counter to 0.
     */
    void Reset()
    {
        mFrame = 0;
        mTotalElapsed = 0.0f;
    }

    /**
     * @brief Stops the animation and resets to first frame.
     */
    void Stop()
    {
        mPaused = true;
        mFrame = 0;
        mTotalElapsed = 0.0f;
    }

    /**
     * @brief Resumes (or starts) the animation from the current frame/time.
     */
    void Play()
    {
        mPaused = false;
    }

    /**
     * @brief Pauses the animation, retaining current frame/time.
     */
    void Pause()
    {
        mPaused = true;
    }

    /**
     * @brief Checks if the animation is currently paused.
     * @return True if paused, false if playing.
     */
    bool IsPaused() const
    {
        return mPaused;
    }

    //-------------------------------------------------------------------------
    // Optional Extension Methods (commented out for demonstration):
    //-------------------------------------------------------------------------
    //
    // void SetFrame(int frameIndex)
    // {
    //     if (frameIndex >= 0 && frameIndex < mFrameCount)
    //     {
    //         mFrame = frameIndex;
    //         mTotalElapsed = 0.0f;
    //     }
    // }
    //
    // bool HasEnded() const
    // {
    //     // If you implement a "play once" logic, you'd check if mFrame 
    //     // reached the last frame and isn't wrapped around.
    //     return false;
    // }

private:
    bool                                                mPaused;        ///< Whether the animation is paused.
    int                                                 mFrame;         ///< Current frame index.
    int                                                 mFrameCount;    ///< Total number of frames in the texture.
    int                                                 mTextureWidth;  ///< Full texture width in pixels.
    int                                                 mTextureHeight; ///< Full texture height in pixels.
    float                                               mTimePerFrame;  ///< Time (seconds) each frame is displayed.
    float                                               mTotalElapsed;  ///< Accumulated time since the last frame change.
    float                                               mDepth;         ///< Depth (z-order) used by SpriteBatch.
    float                                               mRotation;      ///< Rotation in radians.
    DirectX::XMFLOAT2                                   mOrigin;        ///< Rotation origin point.
    DirectX::XMFLOAT2                                   mScale;         ///< 2D scaling factor (x,y).
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    mTexture;       ///< The sprite sheet texture resource.
};

